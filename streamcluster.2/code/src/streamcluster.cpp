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
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + (9769 < rb_input_size ? (uint32_t)rb_input[9769] : 0xdcb5ff74);
        rb_state19.var_0 = (rb_state19.var_0) ^ (14898 < rb_input_size ? (uint32_t)rb_input[14898] : 0x19e85106);
        if ((rb_state19.var_1) == (0x32712f44)) {
            rb_state19.var_14 = 0xc46be950;
            rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_9);
            rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_11);
            rb_state19.var_17 = (rb_state19.var_17) + (42772 < rb_input_size ? (uint32_t)rb_input[42772] : 0x4d0b0dca);
            rb_state19.var_18 = (rb_state19.var_18) - (rb_state19.var_10);
            rb_state19.var_19 = (rb_state19.var_19) ^ (rb_state19.var_14);
            rb_state19.var_20 = (rb_state19.var_20) - (((0xb1e568a9) + (0x34ef9671)) ^ (rb_state19.var_15));
            rb_state19.var_21 = (rb_state19.var_21) + (rb_state19.var_16);
            rb_state19.var_22 = (rb_state19.var_22) ^ (rb_state19.var_17);
            rb_state19.var_23 = (rb_state19.var_23) - (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_19) : (0x6dcea328));
            rb_state19.var_24 = (rb_state19.var_24) - (rb_state19.var_20);
            rb_state19.var_25 = (rb_state19.var_25) - (((0xfbc094b8) - (rb_state19.var_12)) - (rb_state19.var_21));
            rb_state19.var_26 = (rb_state19.var_26) - (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_23) : (0x130c3037));
            rb_state19.var_27 = (rb_state19.var_27) + (((0xf293ea72) + (rb_state19.var_13)) ^ (rb_state19.var_24));
            rb_state19.var_28 = (rb_state19.var_28) - (((((0x30279288) ^ (0xfa65019c)) - (rb_state19.var_14)) + (rb_state19.var_25)) ^ (rb_state19.var_26));
            rb_state19.var_29 = (rb_state19.var_29) - (((0x315254da) ^ (0x58ffc800)) + (rb_state19.var_27));
            rb_state19.var_30 = (rb_state19.var_30) ^ (rb_state19.var_28);
            rb_state19.var_31 = (rb_state19.var_31) - (rb_state19.var_29);
            rb_state19.var_32 = (rb_state19.var_32) ^ (rb_state19.var_30);
            rb_state19.var_33 = (rb_state19.var_33) ^ (((rb_state19.var_31) == (0x0)) ? (rb_state19.var_32) : (0x751b4736));
            rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_33);
            rb_state19.var_2 = rb_state19.var_13;
        }
        if ((rb_state19.var_1) == (0x32712f44)) {
            pthread_mutex_lock(&(rb_state19.lock_49));
            rb_state19.var_35 = 0xb8335a5a;
            rb_state19.var_36 = 0x2fd5e4f0;
            rb_state19.var_37 = (rb_state19.var_37) - (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_16) : (0x22550e2a));
            rb_state19.var_38 = (rb_state19.var_38) ^ (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_18) : (0xddc45590));
            rb_state19.var_39 = (rb_state19.var_39) + (((((0x46a9dfc0) ^ (rb_state19.var_36)) - (rb_state19.var_35)) + (rb_state19.var_20)) ^ (0xf5c16b5f));
            rb_state19.var_40 = (rb_state19.var_40) - (((0xa2818836) ^ (rb_state19.var_15)) - (0xc3e78a64));
            rb_state19.var_41 = (rb_state19.var_41) ^ (((rb_state19.var_37) == (0x0)) ? (rb_state19.var_38) : (0xdd693275));
            rb_state19.var_42 = (rb_state19.var_42) - (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_39) : (0xd3fc3a06));
            rb_state19.var_43 = (rb_state19.var_43) - (rb_state19.var_40);
            rb_state19.var_44 = (rb_state19.var_44) - (((((0x820ba2be) ^ (0xc949309c)) + (rb_state19.var_22)) - (rb_state19.var_42)) + (rb_state19.var_41));
            rb_state19.var_45 = (rb_state19.var_45) + (rb_state19.var_43);
            rb_state19.var_46 = (rb_state19.var_46) - (((0xf4f28d02) + (0x40af151a)) ^ (rb_state19.var_44));
            rb_state19.var_47 = (rb_state19.var_47) - (rb_state19.var_45);
            rb_state19.var_48 = (rb_state19.var_48) + (((rb_state19.var_46) == (0x0)) ? (rb_state19.var_47) : (0x4b261945));
            rb_state19.var_34 = (rb_state19.var_34) - (rb_state19.var_48);
            rb_state19.var_2 = rb_state19.var_34;
            pthread_mutex_unlock(&(rb_state19.lock_49));
        }
        #endif
        float distance = dist(points->p[k], points->p[0], points->dim);
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_0);
        #endif
        points->p[k].cost = distance * points->p[k].weight;
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_1) == (0x32712f44)) {
            if ((rb_state19.var_2) != (0x0)) {
                if (!((rb_state19.var_2) == (rb_state19.var_34))) {
                    racebench_trigger(19);
                }
            }
        }
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
    hiz = loz = 0.0;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x4f66d564)) ? (rb_state11.var_1) : (0xe36333b));
    #endif
    long numberOfPoints = points->num;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x8e5742ab)) {
        pthread_mutex_lock(&(rb_state11.lock_34));
        if ((rb_state11.var_2) != (0x0)) {
            if (!((rb_state11.var_2) == (rb_state11.var_21))) {
                racebench_trigger(11);
            }
        }
        pthread_mutex_unlock(&(rb_state11.lock_34));
    }
    #endif
    long ptDimension = points->dim;

    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
    #endif
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
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xf7428342));
        rb_state3.var_0 = (rb_state3.var_0) - ((0x63116d3f) - (0x58e7bd1f));
        rb_state3.var_1 = (rb_state3.var_1) ^ (0x7353e9cb);
        rb_state3.var_1 = (rb_state3.var_1) - ((0xe67ddc9d) + (0x5bd9e075));
        if ((rb_state3.var_1) == (0xfc5ee5cc)) {
            rb_state3.var_2 = rb_state3.var_0;
        }
        if ((rb_state3.var_1) == (0x4cb54ef5)) {
            rb_state3.var_3 = 0x2aa068e6;
            rb_state3.var_4 = (rb_state3.var_4) ^ (rb_state3.var_4);
            rb_state3.var_5 = (rb_state3.var_5) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x2c31003f));
            rb_state3.var_6 = (rb_state3.var_6) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_5) : (0x263e9528));
            rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_6);
        }
        if ((rb_state3.var_1) == (0x4cb54ef5)) {
            if (!((rb_state3.var_0) == (rb_state3.var_2))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (0x1e974f5e);
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x1e974f5e)) ? (rb_state8.var_0) : (0x7777b5ec));
        rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0xf36c1c52));
        rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0xe55e605e));
        if ((rb_state8.var_0) == (0xce2f784)) {
            rb_state8.var_2 = rb_state8.var_1;
        }
        if ((rb_state8.var_0) == (0xa2f1fcce)) {
            rb_state8.var_3 = 0x9250af5d;
            rb_state8.var_4 = 0x76affa1;
            rb_state8.var_5 = 0x781e9cff;
            rb_state8.var_6 = (rb_state8.var_6) ^ ((0x6a46d694) ^ (0xd2055315));
            rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_5) : (0x5d01256e));
            rb_state8.var_8 = (rb_state8.var_8) ^ (((0x298f8a90) - (rb_state8.var_3)) + (rb_state8.var_3));
            rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_4) : (0xa4ac84af));
            rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0x9b0795d8));
            rb_state8.var_11 = (rb_state8.var_11) ^ (((0xe6036725) ^ (rb_state8.var_8)) - (0xbe927907));
            rb_state8.var_12 = (rb_state8.var_12) + (rb_state8.var_9);
            rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0xe1d2bed5));
            rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_12) : (0xe60f8ab7));
            rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_14) : (0xe1559e61));
            rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_15);
        }
        if ((rb_state8.var_0) == (0xa2f1fcce)) {
            rb_state8.var_17 = !((rb_state8.var_1) == (rb_state8.var_2));
        }
        rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_7) : (0x8d9b66cf));
        rb_state8.var_9 = (rb_state8.var_9) - ((0xa1934857) ^ (rb_state8.var_10));
        if ((rb_state8.var_11) == (0x378a9ab0)) {
            if ((rb_state8.var_18) != (0x0)) {
                if (!((rb_state8.var_18) == (rb_state8.var_45))) {
                    racebench_trigger(8);
                }
            }
        }
        #endif
        myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_11) == (0x378a9ab0)) {
        rb_state8.var_34 = 0xf92f1323;
        rb_state8.var_35 = (rb_state8.var_35) - (((rb_state8.var_29) == (0x0)) ? (rb_state8.var_27) : (0x4d3d994d));
        rb_state8.var_36 = (rb_state8.var_36) - (((0x774bd680) ^ (0xfbf304ab)) ^ (rb_state8.var_34));
        rb_state8.var_37 = (rb_state8.var_37) + (40531 < rb_input_size ? (uint32_t)rb_input[40531] : 0xd1b3d579);
        rb_state8.var_38 = (rb_state8.var_38) - (rb_state8.var_28);
        rb_state8.var_39 = (rb_state8.var_39) - (((rb_state8.var_35) == (0x0)) ? (rb_state8.var_36) : (0xec776846));
        rb_state8.var_40 = (rb_state8.var_40) + (rb_state8.var_37);
        rb_state8.var_41 = (rb_state8.var_41) + (((0x5fbaf04e) + (rb_state8.var_38)) + (rb_state8.var_30));
        rb_state8.var_42 = (rb_state8.var_42) ^ (((0xf5c3051f) + (rb_state8.var_39)) + (rb_state8.var_31));
        rb_state8.var_43 = (rb_state8.var_43) + (((((0xebfa6fbe) + (rb_state8.var_32)) - (0x399fb37c)) + (rb_state8.var_40)) - (rb_state8.var_41));
        rb_state8.var_44 = (rb_state8.var_44) + (((((0xf180b837) + (rb_state8.var_43)) - (0x9863072b)) ^ (rb_state8.var_42)) ^ (rb_state8.var_33));
        rb_state8.var_33 = (rb_state8.var_33) ^ (rb_state8.var_44);
        rb_state8.var_18 = rb_state8.var_33;
    }
    if ((rb_state8.var_11) == (0x378a9ab0)) {
        pthread_mutex_lock(&(rb_state8.lock_52));
        rb_state8.var_46 = 0x9ff51a92;
        rb_state8.var_47 = 0x6c971586;
        rb_state8.var_48 = (rb_state8.var_48) - (((0xb88860ed) - (rb_state8.var_34)) + (0xcbe1b4bb));
        rb_state8.var_49 = (rb_state8.var_49) + (((rb_state8.var_46) == (0x0)) ? (rb_state8.var_47) : (0xa4027c89));
        rb_state8.var_50 = (rb_state8.var_50) ^ (rb_state8.var_48);
        rb_state8.var_51 = (rb_state8.var_51) + (((rb_state8.var_49) == (0x0)) ? (rb_state8.var_50) : (0x9912f89));
        rb_state8.var_45 = (rb_state8.var_45) + (rb_state8.var_51);
        rb_state8.var_18 = rb_state8.var_45;
        pthread_mutex_unlock(&(rb_state8.lock_52));
    }
    #endif
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

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_55));
        if ((rb_state2.var_2) != (0x0)) {
            if (!((rb_state2.var_2) == (rb_state2.var_35))) {
                racebench_trigger(2);
            }
        }
        pthread_mutex_unlock(&(rb_state2.lock_55));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (23376 < rb_input_size ? (uint32_t)rb_input[23376] : 0x72a1ea50);
    rb_state4.var_1 = (rb_state4.var_1) + (54126 < rb_input_size ? (uint32_t)rb_input[54126] : 0x53eb31b1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (42788 < rb_input_size ? (uint32_t)rb_input[42788] : 0xbb5a5d19);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_2);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0xf1)) {
        rb_state9.var_4 = 0xc34a1ec7;
        rb_state9.var_5 = 0x194ac34c;
        rb_state9.var_6 = (rb_state9.var_6) ^ (rb_state9.var_5);
        rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_3);
        rb_state9.var_8 = (rb_state9.var_8) - (((0xbadcdc7a) ^ (0x19bc5e71)) - (rb_state9.var_4));
        rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_7) : (0x6ee63c9c));
        rb_state9.var_10 = (rb_state9.var_10) + (rb_state9.var_8);
        rb_state9.var_11 = (rb_state9.var_11) - (((0xd430a267) ^ (0xefa8c841)) - (rb_state9.var_9));
        rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xb9717fab));
        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_12);
        rb_state9.var_2 = rb_state9.var_3;
    }
    if ((rb_state9.var_1) == (0xf1)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        rb_state9.var_14 = 0x88d0ac95;
        rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_5) : (0xaaee712a));
        rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_6) : (0x9fecccee));
        rb_state9.var_17 = (rb_state9.var_17) + (((((0x668a395c) - (rb_state9.var_9)) + (rb_state9.var_8)) + (rb_state9.var_15)) + (rb_state9.var_4));
        rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_16);
        rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_17) : (0x9f4a2556));
        rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0x67bf4dff));
        rb_state9.var_13 = (rb_state9.var_13) ^ (rb_state9.var_20);
        rb_state9.var_2 = rb_state9.var_13;
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x486ebc05) + (rb_state12.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x3b835c68)) {
        pthread_mutex_lock(&(rb_state13.lock_32));
        if ((rb_state13.var_2) != (0x0)) {
            if (!((rb_state13.var_2) == (rb_state13.var_13))) {
                racebench_trigger(13);
            }
        }
        pthread_mutex_unlock(&(rb_state13.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0x0)) {
        if ((rb_state14.var_2) != (0x0)) {
            if (!((rb_state14.var_2) == (rb_state14.var_28))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x8e8befc8)) {
        rb_state16.var_2 = rb_state16.var_0;
    }
    if ((rb_state16.var_1) == (0xd5d1e791)) {
        if (!((rb_state16.var_0) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x18c750c2)) {
        pthread_mutex_lock(&(rb_state17.lock_10));
        rb_state17.var_3 = 0xe31b4f8a;
        rb_state17.var_4 = (rb_state17.var_4) ^ (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_1) : (0x9d004d52));
        rb_state17.var_5 = (rb_state17.var_5) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_4) : (0x10678f4));
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_5);
        rb_state17.var_1 = rb_state17.var_2;
        pthread_mutex_unlock(&(rb_state17.lock_10));
    }
    if ((rb_state17.var_0) == (0x18c750c2)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0x7ebc674e)) {
        if ((rb_state0.var_2) != (0x0)) {
            if (!((rb_state0.var_2) == (rb_state0.var_20))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ ((0x4c8892f0) + (0x1d1c180e));
    if ((rb_state1.var_1) == (0x0)) {
        rb_state1.var_3 = 0xb82fe314;
        rb_state1.var_4 = (rb_state1.var_4) ^ (((0xa15fc442) ^ (rb_state1.var_3)) + (rb_state1.var_3));
        rb_state1.var_5 = (rb_state1.var_5) + (rb_state1.var_2);
        rb_state1.var_6 = (rb_state1.var_6) ^ (((0x95261c05) - (rb_state1.var_4)) ^ (0x24bc898d));
        rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_5) : (0xad2d3c02));
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0xd40933ed));
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (0xfe3d688b);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xfffffd7b)) {
        rb_state4.var_2 = rb_state4.var_1;
    }
    if ((rb_state4.var_0) == (0xfffffd7b)) {
        if (!((rb_state4.var_1) == (rb_state4.var_2))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_0) == (0x5d2881cf)) ? (rb_state5.var_1) : (0xf3d2592b));
    if ((rb_state5.var_1) == (0x185b4e24)) {
        pthread_mutex_lock(&(rb_state5.lock_13));
        rb_state5.var_2 = rb_state5.var_0;
        pthread_mutex_unlock(&(rb_state5.lock_13));
    }
    if ((rb_state5.var_1) == (0x185b4e24)) {
        if (!((rb_state5.var_0) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (45566 < rb_input_size ? (uint32_t)rb_input[45566] : 0x2cfd7531);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x0)) {
        rb_state10.var_2 = rb_state10.var_0;
    }
    if ((rb_state10.var_1) == (0x0)) {
        if (!((rb_state10.var_0) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x8f7ab510) - (rb_state12.var_0));
    if ((rb_state12.var_0) == (0x8f7ab510)) {
        if ((rb_state12.var_1) != (0x0)) {
            if (!((rb_state12.var_1) == (rb_state12.var_11))) {
                racebench_trigger(12);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0x8e442eb0) + (0x854c78e4));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_9 = 0xf38d4c18;
        rb_state15.var_10 = 0x67d02b63;
        rb_state15.var_11 = (rb_state15.var_11) + (rb_state15.var_5);
        rb_state15.var_12 = (rb_state15.var_12) + (0x75e5e9cb);
        rb_state15.var_13 = (rb_state15.var_13) + (rb_state15.var_9);
        rb_state15.var_14 = (rb_state15.var_14) + (((((0xd274e93) - (rb_state15.var_4)) ^ (rb_state15.var_10)) ^ (rb_state15.var_7)) - (rb_state15.var_6));
        rb_state15.var_15 = (rb_state15.var_15) - (rb_state15.var_11);
        rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_12) : (0xf0504944));
        rb_state15.var_17 = (rb_state15.var_17) - (((0xb8c4f31e) ^ (rb_state15.var_9)) ^ (rb_state15.var_13));
        rb_state15.var_18 = (rb_state15.var_18) - (((0xebc3e79) ^ (rb_state15.var_10)) + (rb_state15.var_14));
        rb_state15.var_19 = (rb_state15.var_19) + (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_15) : (0x3a1d9cb8));
        rb_state15.var_20 = (rb_state15.var_20) + (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_17) : (0x91d4340));
        rb_state15.var_21 = (rb_state15.var_21) - (((0xe6cd98a3) ^ (rb_state15.var_18)) + (0xdd3b4cff));
        rb_state15.var_22 = (rb_state15.var_22) ^ (((((0xea037761) + (rb_state15.var_19)) - (rb_state15.var_20)) + (rb_state15.var_12)) ^ (rb_state15.var_13));
        rb_state15.var_23 = (rb_state15.var_23) + (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_21) : (0x38af7323));
        rb_state15.var_24 = (rb_state15.var_24) ^ (((0xed43caaf) - (rb_state15.var_22)) ^ (0x628d140b));
        rb_state15.var_25 = (rb_state15.var_25) - (((0x1810e6ab) - (rb_state15.var_23)) - (rb_state15.var_15));
        rb_state15.var_26 = (rb_state15.var_26) - (rb_state15.var_24);
        rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0x8cf274df));
        rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_27);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x0)) {
        if ((rb_state18.var_2) != (0x0)) {
            if (!((rb_state18.var_2) == (rb_state18.var_20))) {
                racebench_trigger(18);
            }
        }
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
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_1);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_1) == (0x0)) {
            rb_state2.var_13 = 0x6517ab15;
            rb_state2.var_14 = 0x2ae25fa8;
            rb_state2.var_15 = (rb_state2.var_15) + ((0x3b65a699) - (rb_state2.var_9));
            rb_state2.var_16 = (rb_state2.var_16) + ((0xeadfafad) - (rb_state2.var_10));
            rb_state2.var_17 = (rb_state2.var_17) ^ (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_8) : (0x907fa15d));
            rb_state2.var_18 = (rb_state2.var_18) + (rb_state2.var_14);
            rb_state2.var_19 = (rb_state2.var_19) - (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_15) : (0xed46e608));
            rb_state2.var_20 = (rb_state2.var_20) - (((0x1eaf2c85) + (rb_state2.var_12)) ^ (rb_state2.var_16));
            rb_state2.var_21 = (rb_state2.var_21) - (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_18) : (0x2a7e4641));
            rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_19);
            rb_state2.var_23 = (rb_state2.var_23) - (((0x2c4957be) + (rb_state2.var_13)) ^ (rb_state2.var_20));
            rb_state2.var_24 = (rb_state2.var_24) ^ (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_21) : (0xa0495589));
            rb_state2.var_25 = (rb_state2.var_25) ^ (((((0xe302ab99) + (rb_state2.var_22)) ^ (rb_state2.var_23)) ^ (0x935722ec)) + (rb_state2.var_15));
            rb_state2.var_26 = (rb_state2.var_26) - (rb_state2.var_24);
            rb_state2.var_27 = (rb_state2.var_27) ^ (rb_state2.var_25);
            rb_state2.var_28 = (rb_state2.var_28) ^ (rb_state2.var_26);
            rb_state2.var_29 = (rb_state2.var_29) ^ (rb_state2.var_27);
            rb_state2.var_30 = (rb_state2.var_30) ^ (((0xf573bf59) + (rb_state2.var_28)) + (0xcbe73a24));
            rb_state2.var_31 = (rb_state2.var_31) ^ (rb_state2.var_29);
            rb_state2.var_32 = (rb_state2.var_32) - (((0xf4cfb221) + (rb_state2.var_16)) - (rb_state2.var_30));
            rb_state2.var_33 = (rb_state2.var_33) ^ (((0x7d97ab9) ^ (rb_state2.var_31)) + (0x228e57b));
            rb_state2.var_34 = (rb_state2.var_34) ^ (((((0x740f4fd) ^ (rb_state2.var_33)) - (0xf9761e72)) + (rb_state2.var_32)) + (0x9962b7fa));
            rb_state2.var_12 = (rb_state2.var_12) ^ (rb_state2.var_34);
            rb_state2.var_2 = rb_state2.var_12;
        }
        if ((rb_state2.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state2.lock_55));
            rb_state2.var_36 = 0x9005d3b6;
            rb_state2.var_37 = 0x7f4b5c57;
            rb_state2.var_38 = (rb_state2.var_38) + (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_36) : (0x5a74fa06));
            rb_state2.var_39 = (rb_state2.var_39) + (52436 < rb_input_size ? (uint32_t)rb_input[52436] : 0x12b60f16);
            rb_state2.var_40 = (rb_state2.var_40) + (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_37) : (0x31e4e9b2));
            rb_state2.var_41 = (rb_state2.var_41) + (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_17) : (0x6cacc8f));
            rb_state2.var_42 = (rb_state2.var_42) ^ (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_38) : (0xed32257d));
            rb_state2.var_43 = (rb_state2.var_43) ^ (((0x669c2b0f) ^ (rb_state2.var_22)) ^ (rb_state2.var_39));
            rb_state2.var_44 = (rb_state2.var_44) - (((0x4859a969) + (rb_state2.var_23)) + (rb_state2.var_40));
            rb_state2.var_45 = (rb_state2.var_45) + (((rb_state2.var_41) == (0x0)) ? (rb_state2.var_42) : (0x6492aadb));
            rb_state2.var_46 = (rb_state2.var_46) ^ (((0x2dd4c2ae) ^ (rb_state2.var_24)) - (rb_state2.var_43));
            rb_state2.var_47 = (rb_state2.var_47) ^ (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_44) : (0x6de71a2f));
            rb_state2.var_48 = (rb_state2.var_48) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_45) : (0xc16a77da));
            rb_state2.var_49 = (rb_state2.var_49) ^ (rb_state2.var_46);
            rb_state2.var_50 = (rb_state2.var_50) ^ (((0xeef0b32b) ^ (rb_state2.var_47)) ^ (0x652afd6a));
            rb_state2.var_51 = (rb_state2.var_51) + (rb_state2.var_48);
            rb_state2.var_52 = (rb_state2.var_52) + (((rb_state2.var_49) == (0x0)) ? (rb_state2.var_50) : (0x59f9224d));
            rb_state2.var_53 = (rb_state2.var_53) - (((rb_state2.var_27) == (0x0)) ? (rb_state2.var_51) : (0x96e8c5d));
            rb_state2.var_54 = (rb_state2.var_54) - (((((0x88d84563) - (0x7a504dee)) + (0x28b24a38)) - (rb_state2.var_53)) ^ (rb_state2.var_52));
            rb_state2.var_35 = (rb_state2.var_35) + (rb_state2.var_54);
            rb_state2.var_2 = rb_state2.var_35;
            pthread_mutex_unlock(&(rb_state2.lock_55));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x52671c78));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x8bca8e0f));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (0x4f66d564);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - ((0xb97dea76) + (0xae8399cf));
        if ((rb_state12.var_0) == (0x165076a7)) {
            rb_state12.var_7 = 0xfd24de7d;
            rb_state12.var_8 = (rb_state12.var_8) - (((((0x61b5c4b7) - (rb_state12.var_6)) + (0x3af64022)) ^ (rb_state12.var_7)) + (rb_state12.var_4));
            rb_state12.var_9 = (rb_state12.var_9) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_5) : (0x48c8b3a0));
            rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0x8f303894));
            rb_state12.var_6 = (rb_state12.var_6) ^ (rb_state12.var_10);
            rb_state12.var_1 = rb_state12.var_6;
        }
        if ((rb_state12.var_0) == (0x8f7ab510)) {
            pthread_mutex_lock(&(rb_state12.lock_23));
            rb_state12.var_12 = 0xe3ed99fa;
            rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_10) : (0xefcca359));
            rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_12);
            rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_8) : (0x8fa6926));
            rb_state12.var_16 = (rb_state12.var_16) + (((0x6e400e13) + (0xf36280ac)) - (rb_state12.var_9));
            rb_state12.var_17 = (rb_state12.var_17) - (((((0x1d1d1b04) ^ (0xee16bcd2)) ^ (rb_state12.var_13)) + (rb_state12.var_14)) ^ (0x9f2bf25b));
            rb_state12.var_18 = (rb_state12.var_18) + (rb_state12.var_15);
            rb_state12.var_19 = (rb_state12.var_19) ^ (rb_state12.var_16);
            rb_state12.var_20 = (rb_state12.var_20) + (((((0x4da1b2c2) + (0x18abc60)) + (rb_state12.var_17)) ^ (rb_state12.var_18)) + (rb_state12.var_13));
            rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_19);
            rb_state12.var_22 = (rb_state12.var_22) + (((((0x4134c036) - (0xcff124dc)) ^ (rb_state12.var_21)) - (rb_state12.var_20)) - (rb_state12.var_14));
            rb_state12.var_11 = (rb_state12.var_11) ^ (rb_state12.var_22);
            rb_state12.var_1 = rb_state12.var_11;
            pthread_mutex_unlock(&(rb_state12.lock_23));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (0x4217660);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - ((0xcfbf0725) + (rb_state16.var_0));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ ((0x387a8cf3) + (0xa17c77f7));
        rb_state17.var_0 = (rb_state17.var_0) - ((0xe738af3e) + (rb_state17.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (54505 < rb_input_size ? (uint32_t)rb_input[54505] : 0x163928f5);
        if ((rb_state18.var_1) == (0x0)) {
            rb_state18.var_13 = 0x15e33da8;
            rb_state18.var_14 = (rb_state18.var_14) + (14026 < rb_input_size ? (uint32_t)rb_input[14026] : 0x75f9ae8b);
            rb_state18.var_15 = (rb_state18.var_15) ^ (((0x4932e209) ^ (rb_state18.var_9)) - (rb_state18.var_8));
            rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_13) : (0x42a9dac5));
            rb_state18.var_17 = (rb_state18.var_17) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0x36d8b486));
            rb_state18.var_18 = (rb_state18.var_18) - (rb_state18.var_16);
            rb_state18.var_19 = (rb_state18.var_19) + (((((0x49e57d1) - (rb_state18.var_18)) + (rb_state18.var_10)) - (rb_state18.var_17)) - (rb_state18.var_11));
            rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_19);
            rb_state18.var_2 = rb_state18.var_12;
        }
        if ((rb_state18.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state18.lock_28));
            rb_state18.var_21 = 0x6a2ef7ea;
            rb_state18.var_22 = 0x63fa036;
            rb_state18.var_23 = 0xa3b5bd62;
            rb_state18.var_24 = (rb_state18.var_24) ^ (0x3b2768d8);
            rb_state18.var_25 = (rb_state18.var_25) ^ (((((0xe13ae452) + (rb_state18.var_21)) - (rb_state18.var_23)) + (rb_state18.var_12)) + (rb_state18.var_13));
            rb_state18.var_26 = (rb_state18.var_26) ^ (((((0x264fde6c) ^ (rb_state18.var_14)) + (rb_state18.var_22)) - (0x613e3051)) ^ (rb_state18.var_24));
            rb_state18.var_27 = (rb_state18.var_27) ^ (((((0xe6d3bb8d) + (0x76e40a47)) ^ (rb_state18.var_25)) ^ (rb_state18.var_26)) ^ (rb_state18.var_15));
            rb_state18.var_20 = (rb_state18.var_20) - (rb_state18.var_27);
            rb_state18.var_2 = rb_state18.var_20;
            pthread_mutex_unlock(&(rb_state18.lock_28));
        }
        #endif
        arg[i].points = points;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x7ed82289));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ ((0xca041fe4) ^ (0x4c66101));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ ((0x5b74646b) ^ (rb_state2.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (52305 < rb_input_size ? (uint32_t)rb_input[52305] : 0x65b82046);
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_1) == (0xf1)) {
            if ((rb_state9.var_2) != (0x0)) {
                if (!((rb_state9.var_2) == (rb_state9.var_13))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_1) : (0xff675250));
        if ((rb_state10.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state10.lock_14));
            rb_state10.var_3 = 0x8ce5ee22;
            rb_state10.var_4 = 0xa0a5f486;
            rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0x5e0052d0));
            rb_state10.var_6 = (rb_state10.var_6) - ((0x865f46fa) - (0xa69b84e8));
            rb_state10.var_7 = (rb_state10.var_7) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_2) : (0xc2c628c8));
            rb_state10.var_8 = (rb_state10.var_8) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_4) : (0xba7eb144));
            rb_state10.var_9 = (rb_state10.var_9) ^ (((0x72e87a22) ^ (rb_state10.var_5)) + (0x4e0961d8));
            rb_state10.var_10 = (rb_state10.var_10) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_7) : (0x89ee515c));
            rb_state10.var_11 = (rb_state10.var_11) + (rb_state10.var_8);
            rb_state10.var_12 = (rb_state10.var_12) ^ (((((0x5532e888) + (0x37679221)) - (rb_state10.var_6)) - (rb_state10.var_10)) + (rb_state10.var_9));
            rb_state10.var_13 = (rb_state10.var_13) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_12) : (0x6338a106));
            rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_13);
            pthread_mutex_unlock(&(rb_state10.lock_14));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - ((0x71a8bd55) + (rb_state11.var_0));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x11fcd8cc));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x0)) {
            rb_state14.var_11 = 0x440ea079;
            rb_state14.var_12 = 0x8991e01a;
            rb_state14.var_13 = (rb_state14.var_13) ^ (0xbede0283);
            rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_11) : (0xa6042aa2));
            rb_state14.var_15 = (rb_state14.var_15) ^ (((((0xead3021f) - (0x1bb94587)) ^ (rb_state14.var_6)) ^ (0x8b8946da)) + (rb_state14.var_13));
            rb_state14.var_16 = (rb_state14.var_16) - (((0x81315846) ^ (rb_state14.var_14)) ^ (0x1bf5422d));
            rb_state14.var_17 = (rb_state14.var_17) - (rb_state14.var_15);
            rb_state14.var_18 = (rb_state14.var_18) - (((0xa80eef7a) - (rb_state14.var_16)) ^ (rb_state14.var_7));
            rb_state14.var_19 = (rb_state14.var_19) ^ (((0x10f26998) + (rb_state14.var_8)) - (rb_state14.var_17));
            rb_state14.var_20 = (rb_state14.var_20) - (((0x765b2505) ^ (rb_state14.var_18)) + (0xf2bfeacc));
            rb_state14.var_21 = (rb_state14.var_21) + (((0xdd10d729) ^ (0x65a9a713)) - (rb_state14.var_19));
            rb_state14.var_22 = (rb_state14.var_22) - (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_20) : (0x9a60d0c2));
            rb_state14.var_23 = (rb_state14.var_23) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_21) : (0x6bdfba66));
            rb_state14.var_24 = (rb_state14.var_24) - (((0x52977f9e) ^ (rb_state14.var_11)) + (rb_state14.var_22));
            rb_state14.var_25 = (rb_state14.var_25) - (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_23) : (0x3543b7c2));
            rb_state14.var_26 = (rb_state14.var_26) + (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_24) : (0x5ee3a86b));
            rb_state14.var_27 = (rb_state14.var_27) - (((((0xf9ea8eb3) + (0xbf4558bf)) ^ (rb_state14.var_25)) - (rb_state14.var_26)) ^ (0xadca8688));
            rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_27);
            rb_state14.var_2 = rb_state14.var_10;
        }
        if ((rb_state14.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state14.lock_40));
            rb_state14.var_29 = 0x61c9fe98;
            rb_state14.var_30 = 0x9aa9fb8d;
            rb_state14.var_31 = (rb_state14.var_31) ^ (0x338f7be1);
            rb_state14.var_32 = (rb_state14.var_32) - (0xfa3cc2cc);
            rb_state14.var_33 = (rb_state14.var_33) ^ (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_30) : (0x7cb65387));
            rb_state14.var_34 = (rb_state14.var_34) ^ (((0xd21fcfb3) ^ (0x38cf00a4)) + (rb_state14.var_14));
            rb_state14.var_35 = (rb_state14.var_35) ^ (((((0xd14bfd9f) + (rb_state14.var_31)) - (rb_state14.var_16)) - (0xdb615d06)) + (rb_state14.var_29));
            rb_state14.var_36 = (rb_state14.var_36) ^ (rb_state14.var_32);
            rb_state14.var_37 = (rb_state14.var_37) - (((rb_state14.var_33) == (0x0)) ? (rb_state14.var_34) : (0xc14b1ba6));
            rb_state14.var_38 = (rb_state14.var_38) - (((((0x98d5e6de) ^ (rb_state14.var_36)) ^ (0x264f8ef1)) ^ (0x73a4917a)) ^ (rb_state14.var_35));
            rb_state14.var_39 = (rb_state14.var_39) - (((rb_state14.var_37) == (0x0)) ? (rb_state14.var_38) : (0xe7c98aec));
            rb_state14.var_28 = (rb_state14.var_28) + (rb_state14.var_39);
            rb_state14.var_2 = rb_state14.var_28;
            pthread_mutex_unlock(&(rb_state14.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x56646b7f));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + (0x4745f7c9);
        if ((rb_state16.var_1) == (0xd5d1e791)) {
            rb_state16.var_3 = 0xeaf3e1a;
            rb_state16.var_4 = 0xe7f16356;
            rb_state16.var_5 = (rb_state16.var_5) ^ (((0x148bb678) - (rb_state16.var_3)) + (0xa1b1b809));
            rb_state16.var_6 = (rb_state16.var_6) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_4) : (0x50f93dec));
            rb_state16.var_7 = (rb_state16.var_7) - (rb_state16.var_5);
            rb_state16.var_8 = (rb_state16.var_8) - (rb_state16.var_6);
            rb_state16.var_9 = (rb_state16.var_9) ^ (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_7) : (0x94e19aac));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_8);
            rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_9) : (0xfbcd27c2));
            rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_10) : (0xe419fbc7));
            rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_11);
            rb_state16.var_14 = (rb_state16.var_14) + (((((0x77d11bd3) - (rb_state16.var_12)) ^ (rb_state16.var_6)) + (rb_state16.var_13)) + (0x67725ad2));
            rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x18c750c2)) {
            rb_state17.var_6 = (rb_state17.var_6) + (((((0x6379bafd) + (rb_state17.var_4)) - (rb_state17.var_6)) + (rb_state17.var_5)) - (0x2a541f47));
            rb_state17.var_7 = (rb_state17.var_7) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_7) : (0xa8532028));
            rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_6) : (0x892ee48b));
            rb_state17.var_9 = (rb_state17.var_9) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0x90bafdee));
            rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (0xde804d86);
        rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0xde804c9a)) ? (rb_state18.var_0) : (0xa8313fa1));
        #endif
        arg[i].kmin = kmin;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_1);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (17467 < rb_input_size ? (uint32_t)rb_input[17467] : 0x7fd43c00);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (0xceb2eb80);
        rb_state6.var_1 = (rb_state6.var_1) - (2285 < rb_input_size ? (uint32_t)rb_input[2285] : 0xbc0678ec);
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x3b835c68)) {
            rb_state13.var_6 = 0x629ecf56;
            rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_6) : (0x3854475));
            rb_state13.var_8 = (rb_state13.var_8) - (((0x197437b4) + (rb_state13.var_6)) + (0xfda0de62));
            rb_state13.var_9 = (rb_state13.var_9) - (rb_state13.var_5);
            rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_7) : (0x426ed6cc));
            rb_state13.var_11 = (rb_state13.var_11) - (((((0xb781d113) ^ (rb_state13.var_8)) - (rb_state13.var_8)) - (rb_state13.var_9)) ^ (rb_state13.var_9));
            rb_state13.var_12 = (rb_state13.var_12) + (((((0x6bc4a83c) ^ (rb_state13.var_11)) + (rb_state13.var_10)) ^ (rb_state13.var_10)) ^ (rb_state13.var_11));
            rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_12);
            rb_state13.var_2 = rb_state13.var_5;
        }
        if ((rb_state13.var_0) == (0x3b835c68)) {
            pthread_mutex_lock(&(rb_state13.lock_32));
            rb_state13.var_14 = 0x7957d781;
            rb_state13.var_15 = (rb_state13.var_15) + ((0xaca58691) - (0x27fdb252));
            rb_state13.var_16 = (rb_state13.var_16) + (33819 < rb_input_size ? (uint32_t)rb_input[33819] : 0xed793984);
            rb_state13.var_17 = (rb_state13.var_17) + (0x5db5efff);
            rb_state13.var_18 = (rb_state13.var_18) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_14) : (0x7ec6a1bc));
            rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_13);
            rb_state13.var_20 = (rb_state13.var_20) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_15) : (0xa07c3d3d));
            rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_16);
            rb_state13.var_22 = (rb_state13.var_22) + (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_18) : (0x21d157af));
            rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_19);
            rb_state13.var_24 = (rb_state13.var_24) ^ (rb_state13.var_20);
            rb_state13.var_25 = (rb_state13.var_25) - (rb_state13.var_21);
            rb_state13.var_26 = (rb_state13.var_26) - (((((0xd760ec86) ^ (rb_state13.var_23)) ^ (rb_state13.var_22)) + (rb_state13.var_15)) - (rb_state13.var_16));
            rb_state13.var_27 = (rb_state13.var_27) ^ (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_24) : (0xd0c3846e));
            rb_state13.var_28 = (rb_state13.var_28) ^ (rb_state13.var_25);
            rb_state13.var_29 = (rb_state13.var_29) + (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_27) : (0x33b9e177));
            rb_state13.var_30 = (rb_state13.var_30) ^ (((0xbfdf1f96) ^ (rb_state13.var_28)) + (0xd3910d04));
            rb_state13.var_31 = (rb_state13.var_31) ^ (((((0xd49f1b52) ^ (0x8fe17127)) - (rb_state13.var_29)) - (rb_state13.var_30)) - (0x16561490));
            rb_state13.var_13 = (rb_state13.var_13) + (rb_state13.var_31);
            rb_state13.var_2 = rb_state13.var_13;
            pthread_mutex_unlock(&(rb_state13.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (16983 < rb_input_size ? (uint32_t)rb_input[16983] : 0xc75249af);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ ((0x302b1440) - (rb_state17.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_1);
        #endif
        arg[i].kmax = kmax;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (0xe47d43b2);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_1) == (0x185b4e24)) {
            rb_state5.var_3 = 0x6bdb91f0;
            rb_state5.var_4 = 0x8371d9c0;
            rb_state5.var_5 = (rb_state5.var_5) ^ (15962 < rb_input_size ? (uint32_t)rb_input[15962] : 0xa200284b);
            rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_3);
            rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0xf305b062));
            rb_state5.var_8 = (rb_state5.var_8) + (((((0x882dac91) ^ (rb_state5.var_5)) - (rb_state5.var_6)) + (rb_state5.var_2)) ^ (rb_state5.var_4));
            rb_state5.var_9 = (rb_state5.var_9) ^ (((((0x2a8e78f4) + (rb_state5.var_6)) - (rb_state5.var_5)) - (0xfd0dedb2)) ^ (rb_state5.var_7));
            rb_state5.var_10 = (rb_state5.var_10) - (((((0xa4fe01e7) - (rb_state5.var_7)) - (rb_state5.var_8)) ^ (rb_state5.var_8)) + (0x8fe6a253));
            rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_9) : (0x90cc8cbb));
            rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_11) : (0xce304d88));
            rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x0)) {
            rb_state6.var_2 = rb_state6.var_1;
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_28));
            rb_state15.var_4 = 0x14c7e0e4;
            rb_state15.var_5 = (rb_state15.var_5) ^ (65518 < rb_input_size ? (uint32_t)rb_input[65518] : 0x4c5688e);
            rb_state15.var_6 = (rb_state15.var_6) + (((((0x6759e44d) + (rb_state15.var_3)) ^ (rb_state15.var_2)) ^ (0xcb890aa0)) ^ (0x1f5e9a67));
            rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_5) : (0xaa1f9e65));
            rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_7) : (0x8642baff));
            rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_8);
            rb_state15.var_2 = rb_state15.var_3;
            pthread_mutex_unlock(&(rb_state15.lock_28));
        }
        if ((rb_state15.var_0) == (0x0)) {
            if (!((rb_state15.var_2) == (rb_state15.var_3))) {
                racebench_trigger(15);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
        #endif
        arg[i].pid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - (0x814398b2);
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_1) == (0x0)) {
            rb_state1.var_2 = rb_state1.var_0;
        }
        if ((rb_state1.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_9));
            if (!((rb_state1.var_0) == (rb_state1.var_2))) {
                racebench_trigger(1);
            }
            pthread_mutex_unlock(&(rb_state1.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0xfffffd7b)) {
            rb_state4.var_3 = 0xc6a3036b;
            rb_state4.var_4 = 0x27ccb6e1;
            rb_state4.var_5 = (rb_state4.var_5) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_3) : (0x5b35abf8));
            rb_state4.var_6 = (rb_state4.var_6) - (42266 < rb_input_size ? (uint32_t)rb_input[42266] : 0x5752da5a);
            rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_3) : (0xbce55202));
            rb_state4.var_8 = (rb_state4.var_8) + (((((0x1ac8eb64) ^ (0xae511f12)) - (rb_state4.var_4)) - (rb_state4.var_5)) + (rb_state4.var_5));
            rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_6) : (0x4aef5373));
            rb_state4.var_10 = (rb_state4.var_10) - (((0x641215a4) - (0x45dc4b49)) + (rb_state4.var_7));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0x9d902418));
            rb_state4.var_12 = (rb_state4.var_12) ^ (((0x3ab9edbd) + (rb_state4.var_7)) ^ (rb_state4.var_10));
            rb_state4.var_13 = (rb_state4.var_13) + (((((0x725ccaa0) + (0x861a7c9c)) + (0xfc2529a3)) ^ (rb_state4.var_12)) - (rb_state4.var_11));
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x0)) {
            if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_1);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - ((0x39d8609a) + (rb_state10.var_0));
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x8e5742ab)) {
            rb_state11.var_10 = 0x55fb002e;
            rb_state11.var_11 = 0xe32efb0e;
            rb_state11.var_12 = (rb_state11.var_12) ^ (0xc7c0113e);
            rb_state11.var_13 = (rb_state11.var_13) + (((((0x3873af9e) + (rb_state11.var_11)) - (rb_state11.var_9)) ^ (0x66e2ec3e)) ^ (rb_state11.var_8));
            rb_state11.var_14 = (rb_state11.var_14) - (((0x9d3e5b16) + (rb_state11.var_10)) - (rb_state11.var_10));
            rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_12);
            rb_state11.var_16 = (rb_state11.var_16) - (((0x702c3e34) - (rb_state11.var_13)) + (0x90459668));
            rb_state11.var_17 = (rb_state11.var_17) - (rb_state11.var_14);
            rb_state11.var_18 = (rb_state11.var_18) - (((0x95bac3ef) + (rb_state11.var_15)) - (0xbc77fa85));
            rb_state11.var_19 = (rb_state11.var_19) ^ (((((0x96be58f3) ^ (rb_state11.var_17)) + (0xacfc56b6)) ^ (rb_state11.var_11)) ^ (rb_state11.var_16));
            rb_state11.var_20 = (rb_state11.var_20) - (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x3ca71ef5));
            rb_state11.var_9 = (rb_state11.var_9) ^ (rb_state11.var_20);
            rb_state11.var_2 = rb_state11.var_9;
        }
        if ((rb_state11.var_0) == (0x8e5742ab)) {
            pthread_mutex_lock(&(rb_state11.lock_34));
            rb_state11.var_22 = (rb_state11.var_22) ^ (((((0xba29ff20) ^ (rb_state11.var_14)) ^ (rb_state11.var_13)) - (0x95af5b2c)) ^ (0xc67651ac));
            rb_state11.var_23 = (rb_state11.var_23) ^ (18347 < rb_input_size ? (uint32_t)rb_input[18347] : 0x4be3bda3);
            rb_state11.var_24 = (rb_state11.var_24) - ((0x93ebbd8d) + (0xcad23715));
            rb_state11.var_25 = (rb_state11.var_25) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_22) : (0x62aa1ec8));
            rb_state11.var_26 = (rb_state11.var_26) ^ (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_23) : (0xd49bbe1d));
            rb_state11.var_27 = (rb_state11.var_27) ^ (((((0x83aa6279) + (rb_state11.var_24)) + (rb_state11.var_17)) - (rb_state11.var_16)) - (rb_state11.var_25));
            rb_state11.var_28 = (rb_state11.var_28) ^ (((0x47893a2b) ^ (0xd8425cc)) ^ (rb_state11.var_26));
            rb_state11.var_29 = (rb_state11.var_29) ^ (((0x67438d56) - (rb_state11.var_27)) ^ (0x166cf4bd));
            rb_state11.var_30 = (rb_state11.var_30) + (rb_state11.var_28);
            rb_state11.var_31 = (rb_state11.var_31) ^ (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_29) : (0x1ad78601));
            rb_state11.var_32 = (rb_state11.var_32) ^ (rb_state11.var_30);
            rb_state11.var_33 = (rb_state11.var_33) - (((rb_state11.var_31) == (0x0)) ? (rb_state11.var_32) : (0xa67f88eb));
            rb_state11.var_21 = (rb_state11.var_21) + (rb_state11.var_33);
            rb_state11.var_2 = rb_state11.var_21;
            pthread_mutex_unlock(&(rb_state11.lock_34));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (0xbec100e8);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (34464 < rb_input_size ? (uint32_t)rb_input[34464] : 0xa6a86061);
        #endif
        arg[i].kfinal = kfinal;

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - ((0x6d86c24c) - (0x351783d8));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + ((0x5d2881cf) - (rb_state5.var_0));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) + (59795 < rb_input_size ? (uint32_t)rb_input[59795] : 0x8ae96053);
        rb_state9.var_0 = (rb_state9.var_0) - ((0x66c318f) ^ (0xdb2fedf1));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (23844 < rb_input_size ? (uint32_t)rb_input[23844] : 0x3ca73e69);
        rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xd5fc6408));
        #endif
        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_1) : (0x9f105272));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x277930cb)) ? (rb_state12.var_0) : (0xdc6455ed));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (0x2a7f84a4);
        rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_1);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + (40577 < rb_input_size ? (uint32_t)rb_input[40577] : 0x1d4e26a0);
        #endif
        pthread_create(threads + i, NULL, localSearchSub, (void *)&arg[i]);
#else
        localSearchSub(&arg[0]);
#endif
    }

#ifdef ENABLE_THREADS
    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_1) == (0x7ebc674e)) {
            pthread_mutex_lock(&(rb_state0.lock_27));
            rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_12);
            rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_11);
            rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_16) : (0xc04d71a7));
            rb_state0.var_19 = (rb_state0.var_19) + (((((0x6d8f80c) ^ (rb_state0.var_18)) ^ (rb_state0.var_14)) + (rb_state0.var_17)) + (0x15438365));
            rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_19);
            rb_state0.var_2 = rb_state0.var_15;
            pthread_mutex_unlock(&(rb_state0.lock_27));
        }
        if ((rb_state0.var_1) == (0x7ebc674e)) {
            pthread_mutex_lock(&(rb_state0.lock_27));
            rb_state0.var_21 = 0x421cc900;
            rb_state0.var_22 = 0xd996a23f;
            rb_state0.var_23 = (rb_state0.var_23) ^ (rb_state0.var_15);
            rb_state0.var_24 = (rb_state0.var_24) ^ (((((0x85610e90) ^ (0xa99f7490)) + (rb_state0.var_21)) ^ (0x261e106a)) + (rb_state0.var_22));
            rb_state0.var_25 = (rb_state0.var_25) ^ (rb_state0.var_23);
            rb_state0.var_26 = (rb_state0.var_26) - (((((0xf09b5700) ^ (rb_state0.var_24)) + (rb_state0.var_17)) ^ (rb_state0.var_16)) ^ (rb_state0.var_25));
            rb_state0.var_20 = (rb_state0.var_20) ^ (rb_state0.var_26);
            rb_state0.var_2 = rb_state0.var_20;
            pthread_mutex_unlock(&(rb_state0.lock_27));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (0x70d75b26);
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