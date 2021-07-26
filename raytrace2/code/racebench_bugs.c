#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "racebench.h"
static void _bug0_pattern_clean(void);
static int _bug0_pattern(int part);
static volatile __attribute__((used)) int _bug0_ok;
static volatile __attribute__((used)) int* volatile _bug0_ptr;
static volatile __attribute__((used)) int _bug0_val;
static void _bug0_pattern_clean(void)
{
    _bug0_ok = 0;
    _bug0_ptr = NULL;
    _bug0_val = 0;
}
static __attribute__((optimize(0))) int _bug0_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug0_ok = 1;
        __sync_synchronize();
        _bug0_ptr = &_bug0_val;
    }
    else {
        while (!_bug0_ok)
            pthread_yield();
        if (_bug0_ptr == NULL)
            trigger = 1;
        _bug0_ptr = NULL;
        _bug0_ok = 0;
    }
    return trigger;
}

static void _bug0_pattern_clean(void);
static int _bug0_pattern(int part);
static racebench_bug_helper _bug0_bug_helper;
static racebench_input_helper _bug0_input_helper;
__attribute__((constructor))
static void _bug0_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x98912eb9};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x45d4dd20};
    static racebench_before_helper before_helpers[2] = {};
    _bug0_input_helper.interleave_num = 2;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0xde660bd9;
}
static int _bug0_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[0], 1);
        _bug0_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug0_pattern(part);
}
static int _bug0_input_check(int part)
{
    return rb_input_check(&_bug0_input_helper, 0, part, _bug0_bug_callback);
}
void racebench_bug0(int part)
{
    rb_bug_synchronize(&_bug0_bug_helper, 0, part, _bug0_input_check);
}

static void _bug1_pattern_clean(void);
static int _bug1_pattern(int part);
static pthread_mutex_t _bug1_mutex1;
static pthread_cond_t _bug1_cond1;
static volatile __attribute__((used)) int _bug1_pred;
static volatile __attribute__((used)) int _bug1_sent;
static void _bug1_pattern_clean(void)
{
    pthread_mutex_init(&_bug1_mutex1, NULL);
    pthread_cond_init(&_bug1_cond1, NULL);
    _bug1_pred = 0;
    _bug1_sent = 0;
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug1_mutex1);
        _bug1_pred = 1;
        pthread_cond_signal(&_bug1_cond1);
        _bug1_sent = 1;
        pthread_mutex_unlock(&_bug1_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug1_mutex1);
        if (_bug1_sent) {
            trigger = 1;
        }
        else {
            while (_bug1_pred == 0)
                pthread_cond_wait(&_bug1_cond1, &_bug1_mutex1);
        }
        _bug1_pred = 0;
        pthread_mutex_unlock(&_bug1_mutex1);
    }
    return trigger;
}

static void _bug1_pattern_clean(void);
static int _bug1_pattern(int part);
static racebench_bug_helper _bug1_bug_helper;
static racebench_input_helper _bug1_input_helper;
__attribute__((constructor))
static void _bug1_input_helper_init(void)
{
    static const uint32_t indices[] = {0x220};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug1_input_helper.interleave_num = 0;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xa1182e9e;
}
static int _bug1_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[1], 1);
        _bug1_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug1_pattern(part);
}
static int _bug1_input_check(int part)
{
    return rb_input_check(&_bug1_input_helper, 1, part, _bug1_bug_callback);
}
void racebench_bug1(int part)
{
    rb_bug_synchronize(&_bug1_bug_helper, 1, part, _bug1_input_check);
}

static void _bug2_pattern_clean(void);
static int _bug2_pattern(int part);
static volatile __attribute__((used)) int _bug2_ok;
static volatile __attribute__((used)) int* volatile _bug2_ptr;
static volatile __attribute__((used)) int _bug2_val;
static void _bug2_pattern_clean(void)
{
    _bug2_ok = 0;
    _bug2_ptr = NULL;
    _bug2_val = 0;
}
static __attribute__((optimize(0))) int _bug2_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug2_ok = 1;
        __sync_synchronize();
        _bug2_ptr = &_bug2_val;
    }
    else {
        while (!_bug2_ok)
            pthread_yield();
        if (_bug2_ptr == NULL)
            trigger = 1;
        _bug2_ptr = NULL;
        _bug2_ok = 0;
    }
    return trigger;
}

static void _bug2_pattern_clean(void);
static int _bug2_pattern(int part);
static racebench_bug_helper _bug2_bug_helper;
static racebench_input_helper _bug2_input_helper;
__attribute__((constructor))
static void _bug2_input_helper_init(void)
{
    static const uint32_t indices[] = {0x55};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xc0da1b0f, 0x834c6030};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1204cde};
    static racebench_before_helper before_helpers[3] = {};
    _bug2_input_helper.interleave_num = 3;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0xe546c81d;
}
static int _bug2_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[2], 1);
        _bug2_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug2_pattern(part);
}
static int _bug2_input_check(int part)
{
    return rb_input_check(&_bug2_input_helper, 2, part, _bug2_bug_callback);
}
void racebench_bug2(int part)
{
    rb_bug_synchronize(&_bug2_bug_helper, 2, part, _bug2_input_check);
}

static void _bug3_pattern_clean(void);
static int _bug3_pattern(int part);
static volatile __attribute__((used)) int _bug3_arr[3];
static volatile __attribute__((used)) int _bug3_val;
static volatile __attribute__((used)) int _bug3_pred;
static void _bug3_pattern_clean(void)
{
    _bug3_arr[0] = 0, _bug3_arr[1] = 2, _bug3_arr[2] = 1;
    _bug3_val = 0;
    _bug3_pred = 0;
}
static __attribute__((optimize(0))) int _bug3_pattern(int part)
{
    int trigger = 0;
    _bug3_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug3_val += _bug3_arr[i];
        }
        __sync_synchronize();
        while (!_bug3_pred)
            pthread_yield();
        trigger = _bug3_val != 3;
        _bug3_pred = 0;
        _bug3_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug3_val += _bug3_arr[i];
        }
        __sync_synchronize();
        _bug3_pred = 1;
    }
    return trigger;
}

static void _bug3_pattern_clean(void);
static int _bug3_pattern(int part);
static racebench_bug_helper _bug3_bug_helper;
static racebench_input_helper _bug3_input_helper;
__attribute__((constructor))
static void _bug3_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1e6};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1117c5e};
    static racebench_before_helper before_helpers[0] = {};
    _bug3_input_helper.interleave_num = 0;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xa1117c5e;
}
static int _bug3_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[3], 1);
        _bug3_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug3_pattern(part);
}
static int _bug3_input_check(int part)
{
    return rb_input_check(&_bug3_input_helper, 3, part, _bug3_bug_callback);
}
void racebench_bug3(int part)
{
    rb_bug_synchronize(&_bug3_bug_helper, 3, part, _bug3_input_check);
}

static void _bug4_pattern_clean(void);
static int _bug4_pattern(int part);
static pthread_mutex_t _bug4_mutex1;
static pthread_mutex_t _bug4_mutex2;
static void _bug4_pattern_clean(void)
{
    pthread_mutex_init(&_bug4_mutex1, NULL);
    pthread_mutex_init(&_bug4_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug4_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug4_mutex1);
        if (pthread_mutex_trylock(&_bug4_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug4_mutex2);
        pthread_mutex_unlock(&_bug4_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug4_mutex2);
        if (pthread_mutex_trylock(&_bug4_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug4_mutex1);
        pthread_mutex_unlock(&_bug4_mutex2);
    }
    return trigger;
}

static void _bug4_pattern_clean(void);
static int _bug4_pattern(int part);
static racebench_bug_helper _bug4_bug_helper;
static racebench_input_helper _bug4_input_helper;
__attribute__((constructor))
static void _bug4_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1eb};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118311e};
    static racebench_before_helper before_helpers[0] = {};
    _bug4_input_helper.interleave_num = 0;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xa118311e;
}
static int _bug4_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[4], 1);
        _bug4_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug4_pattern(part);
}
static int _bug4_input_check(int part)
{
    return rb_input_check(&_bug4_input_helper, 4, part, _bug4_bug_callback);
}
void racebench_bug4(int part)
{
    rb_bug_synchronize(&_bug4_bug_helper, 4, part, _bug4_input_check);
}

static void _bug5_pattern_clean(void);
static int _bug5_pattern(int part);
static pthread_mutex_t _bug5_mutex1;
static pthread_mutex_t _bug5_mutex2;
static void _bug5_pattern_clean(void)
{
    pthread_mutex_init(&_bug5_mutex1, NULL);
    pthread_mutex_init(&_bug5_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug5_mutex1);
        if (pthread_mutex_trylock(&_bug5_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug5_mutex2);
        pthread_mutex_unlock(&_bug5_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug5_mutex2);
        if (pthread_mutex_trylock(&_bug5_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug5_mutex1);
        pthread_mutex_unlock(&_bug5_mutex2);
    }
    return trigger;
}

static void _bug5_pattern_clean(void);
static int _bug5_pattern(int part);
static racebench_bug_helper _bug5_bug_helper;
static racebench_input_helper _bug5_input_helper;
__attribute__((constructor))
static void _bug5_input_helper_init(void)
{
    static const uint32_t indices[] = {0x7};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x92e5df1b};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug5_input_helper.interleave_num = 2;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0x33fe0e59;
}
static int _bug5_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[5], 1);
        _bug5_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug5_pattern(part);
}
static int _bug5_input_check(int part)
{
    return rb_input_check(&_bug5_input_helper, 5, part, _bug5_bug_callback);
}
void racebench_bug5(int part)
{
    rb_bug_synchronize(&_bug5_bug_helper, 5, part, _bug5_input_check);
}

static void _bug6_pattern_clean(void);
static int _bug6_pattern(int part);
static pthread_mutex_t _bug6_mutex1;
static volatile __attribute__((used)) int _bug6_first_part;
static void _bug6_pattern_clean(void)
{
    pthread_mutex_init(&_bug6_mutex1, NULL);
    _bug6_first_part = -1;
}
static __attribute__((optimize(0))) int _bug6_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug6_mutex1);
        if (_bug6_first_part == -1)
            _bug6_first_part = part;
        else
            _bug6_first_part = -1;
        pthread_mutex_unlock(&_bug6_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug6_mutex1);
        if (_bug6_first_part == -1) {
            _bug6_first_part = part;
            trigger = 1;
        }
        else
            _bug6_first_part = -1;
        pthread_mutex_unlock(&_bug6_mutex1);
    }
    return trigger;
}

static void _bug6_pattern_clean(void);
static int _bug6_pattern(int part);
static racebench_bug_helper _bug6_bug_helper;
static racebench_input_helper _bug6_input_helper;
__attribute__((constructor))
static void _bug6_input_helper_init(void)
{
    static const uint32_t indices[] = {0x118};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8ccab342, 0x84c19994};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[3] = {};
    _bug6_input_helper.interleave_num = 3;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0xb2a47964;
}
static int _bug6_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[6], 1);
        _bug6_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug6_pattern(part);
}
static int _bug6_input_check(int part)
{
    return rb_input_check(&_bug6_input_helper, 6, part, _bug6_bug_callback);
}
void racebench_bug6(int part)
{
    rb_bug_synchronize(&_bug6_bug_helper, 6, part, _bug6_input_check);
}

static void _bug7_pattern_clean(void);
static int _bug7_pattern(int part);
static pthread_mutex_t _bug7_mutex1;
static volatile __attribute__((used)) int _bug7_val;
static void _bug7_pattern_clean(void)
{
    pthread_mutex_init(&_bug7_mutex1, NULL);
    _bug7_val = 0;
}
static __attribute__((optimize(0))) int _bug7_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug7_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug7_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug7_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug7_pattern_clean(void);
static int _bug7_pattern(int part);
static racebench_bug_helper _bug7_bug_helper;
static racebench_input_helper _bug7_input_helper;
__attribute__((constructor))
static void _bug7_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182f2e};
    static racebench_before_helper before_helpers[1] = {};
    _bug7_input_helper.interleave_num = 1;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xa1182f2e;
}
static int _bug7_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[7], 1);
        _bug7_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug7_pattern(part);
}
static int _bug7_input_check(int part)
{
    return rb_input_check(&_bug7_input_helper, 7, part, _bug7_bug_callback);
}
void racebench_bug7(int part)
{
    rb_bug_synchronize(&_bug7_bug_helper, 7, part, _bug7_input_check);
}

static void _bug8_pattern_clean(void);
static int _bug8_pattern(int part);
static pthread_mutex_t _bug8_mutex1;
static volatile __attribute__((used)) int _bug8_first_part;
static void _bug8_pattern_clean(void)
{
    pthread_mutex_init(&_bug8_mutex1, NULL);
    _bug8_first_part = -1;
}
static __attribute__((optimize(0))) int _bug8_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug8_mutex1);
        if (_bug8_first_part == -1)
            _bug8_first_part = part;
        else
            _bug8_first_part = -1;
        pthread_mutex_unlock(&_bug8_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug8_mutex1);
        if (_bug8_first_part == -1) {
            _bug8_first_part = part;
            trigger = 1;
        }
        else
            _bug8_first_part = -1;
        pthread_mutex_unlock(&_bug8_mutex1);
    }
    return trigger;
}

static void _bug8_pattern_clean(void);
static int _bug8_pattern(int part);
static racebench_bug_helper _bug8_bug_helper;
static racebench_input_helper _bug8_input_helper;
__attribute__((constructor))
static void _bug8_input_helper_init(void)
{
    static const uint32_t indices[] = {0x16};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xbbefe40};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182f7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug8_input_helper.interleave_num = 2;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0xacd72dbe;
}
static int _bug8_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[8], 1);
        _bug8_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug8_pattern(part);
}
static int _bug8_input_check(int part)
{
    return rb_input_check(&_bug8_input_helper, 8, part, _bug8_bug_callback);
}
void racebench_bug8(int part)
{
    rb_bug_synchronize(&_bug8_bug_helper, 8, part, _bug8_input_check);
}

static void _bug9_pattern_clean(void);
static int _bug9_pattern(int part);
static pthread_mutex_t _bug9_mutex1;
static pthread_mutex_t _bug9_mutex2;
static void _bug9_pattern_clean(void)
{
    pthread_mutex_init(&_bug9_mutex1, NULL);
    pthread_mutex_init(&_bug9_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug9_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug9_mutex1);
        if (pthread_mutex_trylock(&_bug9_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug9_mutex2);
        pthread_mutex_unlock(&_bug9_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug9_mutex2);
        if (pthread_mutex_trylock(&_bug9_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug9_mutex1);
        pthread_mutex_unlock(&_bug9_mutex2);
    }
    return trigger;
}

static void _bug9_pattern_clean(void);
static int _bug9_pattern(int part);
static racebench_bug_helper _bug9_bug_helper;
static racebench_input_helper _bug9_input_helper;
__attribute__((constructor))
static void _bug9_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xe1b8b07f};
    static racebench_before_helper before_helpers[0] = {};
    _bug9_input_helper.interleave_num = 0;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0xe1b8b07f;
}
static int _bug9_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[9], 1);
        _bug9_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug9_pattern(part);
}
static int _bug9_input_check(int part)
{
    return rb_input_check(&_bug9_input_helper, 9, part, _bug9_bug_callback);
}
void racebench_bug9(int part)
{
    rb_bug_synchronize(&_bug9_bug_helper, 9, part, _bug9_input_check);
}

static void _bug10_pattern_clean(void);
static int _bug10_pattern(int part);
static pthread_mutex_t _bug10_mutex1;
static pthread_cond_t _bug10_cond1;
static volatile __attribute__((used)) int* volatile _bug10_ptr;
static volatile __attribute__((used)) int _bug10_val;
static volatile __attribute__((used)) int _bug10_pred;
static void _bug10_pattern_clean(void)
{
    pthread_mutex_init(&_bug10_mutex1, NULL);
    pthread_cond_init(&_bug10_cond1, NULL);
    _bug10_ptr = NULL;
    _bug10_val = 0;
    _bug10_pred = 0;
}
static __attribute__((optimize(0))) int _bug10_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug10_pred = 0;
        pthread_cond_signal(&_bug10_cond1);
        pthread_mutex_lock(&_bug10_mutex1);
        _bug10_ptr = &_bug10_val;
        _bug10_pred = 1;
        pthread_mutex_unlock(&_bug10_mutex1);
        pthread_cond_signal(&_bug10_cond1);
    }
    else {
        pthread_mutex_lock(&_bug10_mutex1);
        if (!_bug10_pred)
            pthread_cond_wait(&_bug10_cond1, &_bug10_mutex1);
        if (_bug10_ptr == NULL)
            trigger = 1;
        _bug10_ptr = NULL;
        _bug10_pred = 0;
        pthread_mutex_unlock(&_bug10_mutex1);
    }
    return trigger;
}

static void _bug10_pattern_clean(void);
static int _bug10_pattern(int part);
static racebench_bug_helper _bug10_bug_helper;
static racebench_input_helper _bug10_input_helper;
__attribute__((constructor))
static void _bug10_input_helper_init(void)
{
    static const uint32_t indices[] = {0x21a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xe10ebe9b, 0x72c77397};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182b6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug10_input_helper.interleave_num = 3;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0xf4ee5da0;
}
static int _bug10_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[10], 1);
        _bug10_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug10_pattern(part);
}
static int _bug10_input_check(int part)
{
    return rb_input_check(&_bug10_input_helper, 10, part, _bug10_bug_callback);
}
void racebench_bug10(int part)
{
    rb_bug_synchronize(&_bug10_bug_helper, 10, part, _bug10_input_check);
}

static void _bug11_pattern_clean(void);
static int _bug11_pattern(int part);
static pthread_mutex_t _bug11_mutex1;
static pthread_cond_t _bug11_cond1;
static volatile __attribute__((used)) int* volatile _bug11_ptr;
static volatile __attribute__((used)) int _bug11_val;
static volatile __attribute__((used)) int _bug11_pred;
static void _bug11_pattern_clean(void)
{
    pthread_mutex_init(&_bug11_mutex1, NULL);
    pthread_cond_init(&_bug11_cond1, NULL);
    _bug11_ptr = NULL;
    _bug11_val = 0;
    _bug11_pred = 0;
}
static __attribute__((optimize(0))) int _bug11_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug11_pred = 0;
        pthread_cond_signal(&_bug11_cond1);
        pthread_mutex_lock(&_bug11_mutex1);
        _bug11_ptr = &_bug11_val;
        _bug11_pred = 1;
        pthread_mutex_unlock(&_bug11_mutex1);
        pthread_cond_signal(&_bug11_cond1);
    }
    else {
        pthread_mutex_lock(&_bug11_mutex1);
        if (!_bug11_pred)
            pthread_cond_wait(&_bug11_cond1, &_bug11_mutex1);
        if (_bug11_ptr == NULL)
            trigger = 1;
        _bug11_ptr = NULL;
        _bug11_pred = 0;
        pthread_mutex_unlock(&_bug11_mutex1);
    }
    return trigger;
}

static void _bug11_pattern_clean(void);
static int _bug11_pattern(int part);
static racebench_bug_helper _bug11_bug_helper;
static racebench_input_helper _bug11_input_helper;
__attribute__((constructor))
static void _bug11_input_helper_init(void)
{
    static const uint32_t indices[] = {0x36};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118b0ce};
    static racebench_before_helper before_helpers[0] = {};
    _bug11_input_helper.interleave_num = 0;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0xa118b0ce;
}
static int _bug11_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[11], 1);
        _bug11_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug11_pattern(part);
}
static int _bug11_input_check(int part)
{
    return rb_input_check(&_bug11_input_helper, 11, part, _bug11_bug_callback);
}
void racebench_bug11(int part)
{
    rb_bug_synchronize(&_bug11_bug_helper, 11, part, _bug11_input_check);
}

static void _bug12_pattern_clean(void);
static int _bug12_pattern(int part);
static volatile __attribute__((used)) int _bug12_ok;
static volatile __attribute__((used)) int* volatile _bug12_ptr;
static volatile __attribute__((used)) int _bug12_val;
static void _bug12_pattern_clean(void)
{
    _bug12_ok = 0;
    _bug12_ptr = NULL;
    _bug12_val = 0;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug12_ok = 1;
        __sync_synchronize();
        _bug12_ptr = &_bug12_val;
    }
    else {
        while (!_bug12_ok)
            pthread_yield();
        if (_bug12_ptr == NULL)
            trigger = 1;
        _bug12_ptr = NULL;
        _bug12_ok = 0;
    }
    return trigger;
}

static void _bug12_pattern_clean(void);
static int _bug12_pattern(int part);
static racebench_bug_helper _bug12_bug_helper;
static racebench_input_helper _bug12_input_helper;
__attribute__((constructor))
static void _bug12_input_helper_init(void)
{
    static const uint32_t indices[] = {0x68};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x79cb443c, 0xa7ad1dd6};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug12_input_helper.interleave_num = 3;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xc2908e90;
}
static int _bug12_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[12], 1);
        _bug12_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug12_pattern(part);
}
static int _bug12_input_check(int part)
{
    return rb_input_check(&_bug12_input_helper, 12, part, _bug12_bug_callback);
}
void racebench_bug12(int part)
{
    rb_bug_synchronize(&_bug12_bug_helper, 12, part, _bug12_input_check);
}

static void _bug13_pattern_clean(void);
static int _bug13_pattern(int part);
static pthread_mutex_t _bug13_mutex1;
static pthread_cond_t _bug13_cond1;
static volatile __attribute__((used)) int* volatile _bug13_ptr;
static volatile __attribute__((used)) int _bug13_val;
static volatile __attribute__((used)) int _bug13_pred;
static void _bug13_pattern_clean(void)
{
    pthread_mutex_init(&_bug13_mutex1, NULL);
    pthread_cond_init(&_bug13_cond1, NULL);
    _bug13_ptr = NULL;
    _bug13_val = 0;
    _bug13_pred = 0;
}
static __attribute__((optimize(0))) int _bug13_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug13_pred = 0;
        pthread_cond_signal(&_bug13_cond1);
        pthread_mutex_lock(&_bug13_mutex1);
        _bug13_ptr = &_bug13_val;
        _bug13_pred = 1;
        pthread_mutex_unlock(&_bug13_mutex1);
        pthread_cond_signal(&_bug13_cond1);
    }
    else {
        pthread_mutex_lock(&_bug13_mutex1);
        if (!_bug13_pred)
            pthread_cond_wait(&_bug13_cond1, &_bug13_mutex1);
        if (_bug13_ptr == NULL)
            trigger = 1;
        _bug13_ptr = NULL;
        _bug13_pred = 0;
        pthread_mutex_unlock(&_bug13_mutex1);
    }
    return trigger;
}

static void _bug13_pattern_clean(void);
static int _bug13_pattern(int part);
static racebench_bug_helper _bug13_bug_helper;
static racebench_input_helper _bug13_input_helper;
__attribute__((constructor))
static void _bug13_input_helper_init(void)
{
    static const uint32_t indices[] = {0x206};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb6ca4f04};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xe4096cbf};
    static racebench_before_helper before_helpers[2] = {};
    _bug13_input_helper.interleave_num = 2;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0x9ad3bbc3;
}
static int _bug13_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[13], 1);
        _bug13_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug13_pattern(part);
}
static int _bug13_input_check(int part)
{
    return rb_input_check(&_bug13_input_helper, 13, part, _bug13_bug_callback);
}
void racebench_bug13(int part)
{
    rb_bug_synchronize(&_bug13_bug_helper, 13, part, _bug13_input_check);
}

static void _bug14_pattern_clean(void);
static int _bug14_pattern(int part);
static volatile __attribute__((used)) int _bug14_arr[3];
static volatile __attribute__((used)) int _bug14_val;
static volatile __attribute__((used)) int _bug14_pred;
static void _bug14_pattern_clean(void)
{
    _bug14_arr[0] = 0, _bug14_arr[1] = 2, _bug14_arr[2] = 1;
    _bug14_val = 0;
    _bug14_pred = 0;
}
static __attribute__((optimize(0))) int _bug14_pattern(int part)
{
    int trigger = 0;
    _bug14_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug14_val += _bug14_arr[i];
        }
        __sync_synchronize();
        while (!_bug14_pred)
            pthread_yield();
        trigger = _bug14_val != 3;
        _bug14_pred = 0;
        _bug14_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug14_val += _bug14_arr[i];
        }
        __sync_synchronize();
        _bug14_pred = 1;
    }
    return trigger;
}

static void _bug14_pattern_clean(void);
static int _bug14_pattern(int part);
static racebench_bug_helper _bug14_bug_helper;
static racebench_input_helper _bug14_input_helper;
__attribute__((constructor))
static void _bug14_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10d};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug14_input_helper.interleave_num = 0;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xa1182c6e;
}
static int _bug14_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[14], 1);
        _bug14_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug14_pattern(part);
}
static int _bug14_input_check(int part)
{
    return rb_input_check(&_bug14_input_helper, 14, part, _bug14_bug_callback);
}
void racebench_bug14(int part)
{
    rb_bug_synchronize(&_bug14_bug_helper, 14, part, _bug14_input_check);
}

static void _bug15_pattern_clean(void);
static int _bug15_pattern(int part);
static pthread_mutex_t _bug15_mutex1;
static pthread_mutex_t _bug15_mutex2;
static void _bug15_pattern_clean(void)
{
    pthread_mutex_init(&_bug15_mutex1, NULL);
    pthread_mutex_init(&_bug15_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug15_mutex1);
        if (pthread_mutex_trylock(&_bug15_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug15_mutex2);
        pthread_mutex_unlock(&_bug15_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug15_mutex2);
        if (pthread_mutex_trylock(&_bug15_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug15_mutex1);
        pthread_mutex_unlock(&_bug15_mutex2);
    }
    return trigger;
}

static void _bug15_pattern_clean(void);
static int _bug15_pattern(int part);
static racebench_bug_helper _bug15_bug_helper;
static racebench_input_helper _bug15_input_helper;
__attribute__((constructor))
static void _bug15_input_helper_init(void)
{
    static const uint32_t indices[] = {0x9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x9b079d43, 0xc1fed193};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xc8686e5f};
    static racebench_before_helper before_helpers[3] = {};
    _bug15_input_helper.interleave_num = 3;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0x256edd35;
}
static int _bug15_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[15], 1);
        _bug15_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug15_pattern(part);
}
static int _bug15_input_check(int part)
{
    return rb_input_check(&_bug15_input_helper, 15, part, _bug15_bug_callback);
}
void racebench_bug15(int part)
{
    rb_bug_synchronize(&_bug15_bug_helper, 15, part, _bug15_input_check);
}

static void _bug16_pattern_clean(void);
static int _bug16_pattern(int part);
static pthread_mutex_t _bug16_mutex1;
static pthread_mutex_t _bug16_mutex2;
static void _bug16_pattern_clean(void)
{
    pthread_mutex_init(&_bug16_mutex1, NULL);
    pthread_mutex_init(&_bug16_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug16_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug16_mutex1);
        if (pthread_mutex_trylock(&_bug16_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug16_mutex2);
        pthread_mutex_unlock(&_bug16_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug16_mutex2);
        if (pthread_mutex_trylock(&_bug16_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug16_mutex1);
        pthread_mutex_unlock(&_bug16_mutex2);
    }
    return trigger;
}

static void _bug16_pattern_clean(void);
static int _bug16_pattern(int part);
static racebench_bug_helper _bug16_bug_helper;
static racebench_input_helper _bug16_input_helper;
__attribute__((constructor))
static void _bug16_input_helper_init(void)
{
    static const uint32_t indices[] = {0x236};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x78d0f48b};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1139bee};
    static racebench_before_helper before_helpers[2] = {};
    _bug16_input_helper.interleave_num = 2;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0x19e49079;
}
static int _bug16_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[16], 1);
        _bug16_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug16_pattern(part);
}
static int _bug16_input_check(int part)
{
    return rb_input_check(&_bug16_input_helper, 16, part, _bug16_bug_callback);
}
void racebench_bug16(int part)
{
    rb_bug_synchronize(&_bug16_bug_helper, 16, part, _bug16_input_check);
}

static void _bug17_pattern_clean(void);
static int _bug17_pattern(int part);
static pthread_mutex_t _bug17_mutex1;
static pthread_mutex_t _bug17_mutex2;
static void _bug17_pattern_clean(void)
{
    pthread_mutex_init(&_bug17_mutex1, NULL);
    pthread_mutex_init(&_bug17_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug17_mutex1);
        if (pthread_mutex_trylock(&_bug17_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug17_mutex2);
        pthread_mutex_unlock(&_bug17_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug17_mutex2);
        if (pthread_mutex_trylock(&_bug17_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug17_mutex1);
        pthread_mutex_unlock(&_bug17_mutex2);
    }
    return trigger;
}

static void _bug17_pattern_clean(void);
static int _bug17_pattern(int part);
static racebench_bug_helper _bug17_bug_helper;
static racebench_input_helper _bug17_input_helper;
__attribute__((constructor))
static void _bug17_input_helper_init(void)
{
    static const uint32_t indices[] = {0x23a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x8509256c};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1116bbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug17_input_helper.interleave_num = 2;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0x261a912a;
}
static int _bug17_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[17], 1);
        _bug17_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug17_pattern(part);
}
static int _bug17_input_check(int part)
{
    return rb_input_check(&_bug17_input_helper, 17, part, _bug17_bug_callback);
}
void racebench_bug17(int part)
{
    rb_bug_synchronize(&_bug17_bug_helper, 17, part, _bug17_input_check);
}

static void _bug18_pattern_clean(void);
static int _bug18_pattern(int part);
static pthread_mutex_t _bug18_mutex1;
static pthread_cond_t _bug18_cond1;
static volatile __attribute__((used)) int* volatile _bug18_ptr;
static volatile __attribute__((used)) int _bug18_val;
static volatile __attribute__((used)) int _bug18_pred;
static void _bug18_pattern_clean(void)
{
    pthread_mutex_init(&_bug18_mutex1, NULL);
    pthread_cond_init(&_bug18_cond1, NULL);
    _bug18_ptr = NULL;
    _bug18_val = 0;
    _bug18_pred = 0;
}
static __attribute__((optimize(0))) int _bug18_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug18_pred = 0;
        pthread_cond_signal(&_bug18_cond1);
        pthread_mutex_lock(&_bug18_mutex1);
        _bug18_ptr = &_bug18_val;
        _bug18_pred = 1;
        pthread_mutex_unlock(&_bug18_mutex1);
        pthread_cond_signal(&_bug18_cond1);
    }
    else {
        pthread_mutex_lock(&_bug18_mutex1);
        if (!_bug18_pred)
            pthread_cond_wait(&_bug18_cond1, &_bug18_mutex1);
        if (_bug18_ptr == NULL)
            trigger = 1;
        _bug18_ptr = NULL;
        _bug18_pred = 0;
        pthread_mutex_unlock(&_bug18_mutex1);
    }
    return trigger;
}

static void _bug18_pattern_clean(void);
static int _bug18_pattern(int part);
static racebench_bug_helper _bug18_bug_helper;
static racebench_input_helper _bug18_input_helper;
__attribute__((constructor))
static void _bug18_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1af};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1156f8e};
    static racebench_before_helper before_helpers[0] = {};
    _bug18_input_helper.interleave_num = 0;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0xa1156f8e;
}
static int _bug18_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[18], 1);
        _bug18_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug18_pattern(part);
}
static int _bug18_input_check(int part)
{
    return rb_input_check(&_bug18_input_helper, 18, part, _bug18_bug_callback);
}
void racebench_bug18(int part)
{
    rb_bug_synchronize(&_bug18_bug_helper, 18, part, _bug18_input_check);
}

static void _bug19_pattern_clean(void);
static int _bug19_pattern(int part);
static pthread_mutex_t _bug19_mutex1;
static pthread_mutex_t _bug19_mutex2;
static void _bug19_pattern_clean(void)
{
    pthread_mutex_init(&_bug19_mutex1, NULL);
    pthread_mutex_init(&_bug19_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug19_mutex1);
        if (pthread_mutex_trylock(&_bug19_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug19_mutex2);
        pthread_mutex_unlock(&_bug19_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug19_mutex2);
        if (pthread_mutex_trylock(&_bug19_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug19_mutex1);
        pthread_mutex_unlock(&_bug19_mutex2);
    }
    return trigger;
}

static void _bug19_pattern_clean(void);
static int _bug19_pattern(int part);
static racebench_bug_helper _bug19_bug_helper;
static racebench_input_helper _bug19_input_helper;
__attribute__((constructor))
static void _bug19_input_helper_init(void)
{
    static const uint32_t indices[] = {0x86};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa52aa1d6};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x37321c53};
    static racebench_before_helper before_helpers[2] = {};
    _bug19_input_helper.interleave_num = 2;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0xdc5cbe29;
}
static int _bug19_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[19], 1);
        _bug19_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug19_pattern(part);
}
static int _bug19_input_check(int part)
{
    return rb_input_check(&_bug19_input_helper, 19, part, _bug19_bug_callback);
}
void racebench_bug19(int part)
{
    rb_bug_synchronize(&_bug19_bug_helper, 19, part, _bug19_input_check);
}

static void _bug20_pattern_clean(void);
static int _bug20_pattern(int part);
static volatile __attribute__((used)) int _bug20_ok;
static volatile __attribute__((used)) int* volatile _bug20_ptr;
static volatile __attribute__((used)) int _bug20_val;
static void _bug20_pattern_clean(void)
{
    _bug20_ok = 0;
    _bug20_ptr = NULL;
    _bug20_val = 0;
}
static __attribute__((optimize(0))) int _bug20_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug20_ok = 1;
        __sync_synchronize();
        _bug20_ptr = &_bug20_val;
    }
    else {
        while (!_bug20_ok)
            pthread_yield();
        if (_bug20_ptr == NULL)
            trigger = 1;
        _bug20_ptr = NULL;
        _bug20_ok = 0;
    }
    return trigger;
}

static void _bug20_pattern_clean(void);
static int _bug20_pattern(int part);
static racebench_bug_helper _bug20_bug_helper;
static racebench_input_helper _bug20_input_helper;
__attribute__((constructor))
static void _bug20_input_helper_init(void)
{
    static const uint32_t indices[] = {0x207};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa118909e};
    static racebench_before_helper before_helpers[1] = {};
    _bug20_input_helper.interleave_num = 1;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xa118909e;
}
static int _bug20_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[20], 1);
        _bug20_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug20_pattern(part);
}
static int _bug20_input_check(int part)
{
    return rb_input_check(&_bug20_input_helper, 20, part, _bug20_bug_callback);
}
void racebench_bug20(int part)
{
    rb_bug_synchronize(&_bug20_bug_helper, 20, part, _bug20_input_check);
}

static void _bug21_pattern_clean(void);
static int _bug21_pattern(int part);
static pthread_mutex_t _bug21_mutex1;
static volatile __attribute__((used)) int _bug21_first_part;
static volatile __attribute__((used)) int _bug21_released;
static void _bug21_pattern_clean(void)
{
    pthread_mutex_init(&_bug21_mutex1, NULL);
    _bug21_first_part = -1;
    _bug21_released = -1;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug21_mutex1) == 0)
                break;
            if (_bug21_released == 0) {
                _bug21_first_part = -1;
                _bug21_released = -1;
                return 1;
            }
        }
        if (_bug21_first_part == -1)
            _bug21_first_part = part;
        else {
            _bug21_first_part = -1;
            while (_bug21_released == -1) {}
            _bug21_released = -1;
        }
        pthread_mutex_unlock(&_bug21_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug21_mutex1);
        if (_bug21_first_part == -1) {
            _bug21_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug21_mutex1);
                _bug21_released = 1;
            }
            else {
                _bug21_released = 0;
                while (_bug21_released != -1) {}
                pthread_mutex_unlock(&_bug21_mutex1);
                return 1;
            }
        }
        else {
            _bug21_first_part = -1;
            pthread_mutex_unlock(&_bug21_mutex1);
        }
    }
    return 0;
}

static void _bug21_pattern_clean(void);
static int _bug21_pattern(int part);
static racebench_bug_helper _bug21_bug_helper;
static racebench_input_helper _bug21_input_helper;
__attribute__((constructor))
static void _bug21_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xf715417f};
    static racebench_before_helper before_helpers[1] = {};
    _bug21_input_helper.interleave_num = 1;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0xf715417f;
}
static int _bug21_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[21], 1);
        _bug21_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug21_pattern(part);
}
static int _bug21_input_check(int part)
{
    return rb_input_check(&_bug21_input_helper, 21, part, _bug21_bug_callback);
}
void racebench_bug21(int part)
{
    rb_bug_synchronize(&_bug21_bug_helper, 21, part, _bug21_input_check);
}

static void _bug22_pattern_clean(void);
static int _bug22_pattern(int part);
static pthread_mutex_t _bug22_mutex1;
static volatile __attribute__((used)) int* volatile _bug22_ptr;
static volatile __attribute__((used)) int _bug22_val;
static void _bug22_pattern_clean(void)
{
    pthread_mutex_init(&_bug22_mutex1, NULL);
    _bug22_ptr = NULL;
    _bug22_val = 0;
}
static __attribute__((optimize(0))) int _bug22_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug22_mutex1);
        _bug22_ptr = &_bug22_val;
        pthread_mutex_unlock(&_bug22_mutex1);
        pthread_mutex_lock(&_bug22_mutex1);
        if (_bug22_ptr == NULL)
            trigger = 1;
        _bug22_ptr = NULL;
        pthread_mutex_unlock(&_bug22_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug22_mutex1);
        _bug22_ptr = &_bug22_val;
        if (_bug22_ptr == NULL)
            trigger = 1;
        _bug22_ptr = NULL;
        pthread_mutex_unlock(&_bug22_mutex1);
    }
    return trigger;
}

static void _bug22_pattern_clean(void);
static int _bug22_pattern(int part);
static racebench_bug_helper _bug22_bug_helper;
static racebench_input_helper _bug22_input_helper;
__attribute__((constructor))
static void _bug22_input_helper_init(void)
{
    static const uint32_t indices[] = {0x21b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x184a2796};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11830de};
    static racebench_before_helper before_helpers[2] = {};
    _bug22_input_helper.interleave_num = 2;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0xb9625874;
}
static int _bug22_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[22], 1);
        _bug22_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug22_pattern(part);
}
static int _bug22_input_check(int part)
{
    return rb_input_check(&_bug22_input_helper, 22, part, _bug22_bug_callback);
}
void racebench_bug22(int part)
{
    rb_bug_synchronize(&_bug22_bug_helper, 22, part, _bug22_input_check);
}

static void _bug23_pattern_clean(void);
static int _bug23_pattern(int part);
static pthread_mutex_t _bug23_mutex1;
static volatile __attribute__((used)) int _bug23_first_part;
static void _bug23_pattern_clean(void)
{
    pthread_mutex_init(&_bug23_mutex1, NULL);
    _bug23_first_part = -1;
}
static __attribute__((optimize(0))) int _bug23_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug23_mutex1);
        if (_bug23_first_part == -1)
            _bug23_first_part = part;
        else
            _bug23_first_part = -1;
        pthread_mutex_unlock(&_bug23_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug23_mutex1);
        if (_bug23_first_part == -1) {
            _bug23_first_part = part;
            trigger = 1;
        }
        else
            _bug23_first_part = -1;
        pthread_mutex_unlock(&_bug23_mutex1);
    }
    return trigger;
}

static void _bug23_pattern_clean(void);
static int _bug23_pattern(int part);
static racebench_bug_helper _bug23_bug_helper;
static racebench_input_helper _bug23_input_helper;
__attribute__((constructor))
static void _bug23_input_helper_init(void)
{
    static const uint32_t indices[] = {0x21a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1147fce};
    static racebench_before_helper before_helpers[1] = {};
    _bug23_input_helper.interleave_num = 1;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0xa1147fce;
}
static int _bug23_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[23], 1);
        _bug23_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug23_pattern(part);
}
static int _bug23_input_check(int part)
{
    return rb_input_check(&_bug23_input_helper, 23, part, _bug23_bug_callback);
}
void racebench_bug23(int part)
{
    rb_bug_synchronize(&_bug23_bug_helper, 23, part, _bug23_input_check);
}

static void _bug24_pattern_clean(void);
static int _bug24_pattern(int part);
static pthread_mutex_t _bug24_mutex1;
static volatile __attribute__((used)) int _bug24_first_part;
static void _bug24_pattern_clean(void)
{
    pthread_mutex_init(&_bug24_mutex1, NULL);
    _bug24_first_part = -1;
}
static __attribute__((optimize(0))) int _bug24_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug24_mutex1);
        if (_bug24_first_part == -1)
            _bug24_first_part = part;
        else
            _bug24_first_part = -1;
        pthread_mutex_unlock(&_bug24_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug24_mutex1);
        if (_bug24_first_part == -1) {
            _bug24_first_part = part;
            trigger = 1;
        }
        else
            _bug24_first_part = -1;
        pthread_mutex_unlock(&_bug24_mutex1);
    }
    return trigger;
}

static void _bug24_pattern_clean(void);
static int _bug24_pattern(int part);
static racebench_bug_helper _bug24_bug_helper;
static racebench_input_helper _bug24_input_helper;
__attribute__((constructor))
static void _bug24_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1e8};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xbf00ef87};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182e6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug24_input_helper.interleave_num = 2;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0x60191df5;
}
static int _bug24_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[24], 1);
        _bug24_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug24_pattern(part);
}
static int _bug24_input_check(int part)
{
    return rb_input_check(&_bug24_input_helper, 24, part, _bug24_bug_callback);
}
void racebench_bug24(int part)
{
    rb_bug_synchronize(&_bug24_bug_helper, 24, part, _bug24_input_check);
}

static void _bug25_pattern_clean(void);
static int _bug25_pattern(int part);
static pthread_mutex_t _bug25_mutex1;
static pthread_mutex_t _bug25_mutex2;
static void _bug25_pattern_clean(void)
{
    pthread_mutex_init(&_bug25_mutex1, NULL);
    pthread_mutex_init(&_bug25_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug25_mutex1);
        if (pthread_mutex_trylock(&_bug25_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug25_mutex2);
        pthread_mutex_unlock(&_bug25_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug25_mutex2);
        if (pthread_mutex_trylock(&_bug25_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug25_mutex1);
        pthread_mutex_unlock(&_bug25_mutex2);
    }
    return trigger;
}

static void _bug25_pattern_clean(void);
static int _bug25_pattern(int part);
static racebench_bug_helper _bug25_bug_helper;
static racebench_input_helper _bug25_input_helper;
__attribute__((constructor))
static void _bug25_input_helper_init(void)
{
    static const uint32_t indices[] = {0x120};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug25_input_helper.interleave_num = 1;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xa1182c4e;
}
static int _bug25_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[25], 1);
        _bug25_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug25_pattern(part);
}
static int _bug25_input_check(int part)
{
    return rb_input_check(&_bug25_input_helper, 25, part, _bug25_bug_callback);
}
void racebench_bug25(int part)
{
    rb_bug_synchronize(&_bug25_bug_helper, 25, part, _bug25_input_check);
}

static void _bug26_pattern_clean(void);
static int _bug26_pattern(int part);
static volatile __attribute__((used)) int _bug26_ok;
static volatile __attribute__((used)) int* volatile _bug26_ptr;
static volatile __attribute__((used)) int _bug26_val;
static void _bug26_pattern_clean(void)
{
    _bug26_ok = 0;
    _bug26_ptr = NULL;
    _bug26_val = 0;
}
static __attribute__((optimize(0))) int _bug26_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug26_ok = 1;
        __sync_synchronize();
        _bug26_ptr = &_bug26_val;
    }
    else {
        while (!_bug26_ok)
            pthread_yield();
        if (_bug26_ptr == NULL)
            trigger = 1;
        _bug26_ptr = NULL;
        _bug26_ok = 0;
    }
    return trigger;
}

static void _bug26_pattern_clean(void);
static int _bug26_pattern(int part);
static racebench_bug_helper _bug26_bug_helper;
static racebench_input_helper _bug26_input_helper;
__attribute__((constructor))
static void _bug26_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1fc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x547509ef};
    static racebench_before_helper before_helpers[1] = {};
    _bug26_input_helper.interleave_num = 1;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0x547509ef;
}
static int _bug26_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[26], 1);
        _bug26_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug26_pattern(part);
}
static int _bug26_input_check(int part)
{
    return rb_input_check(&_bug26_input_helper, 26, part, _bug26_bug_callback);
}
void racebench_bug26(int part)
{
    rb_bug_synchronize(&_bug26_bug_helper, 26, part, _bug26_input_check);
}

static void _bug27_pattern_clean(void);
static int _bug27_pattern(int part);
static volatile __attribute__((used)) int _bug27_arr[3];
static volatile __attribute__((used)) int _bug27_val;
static volatile __attribute__((used)) int _bug27_pred;
static void _bug27_pattern_clean(void)
{
    _bug27_arr[0] = 0, _bug27_arr[1] = 2, _bug27_arr[2] = 1;
    _bug27_val = 0;
    _bug27_pred = 0;
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    int trigger = 0;
    _bug27_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug27_val += _bug27_arr[i];
        }
        __sync_synchronize();
        while (!_bug27_pred)
            pthread_yield();
        trigger = _bug27_val != 3;
        _bug27_pred = 0;
        _bug27_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug27_val += _bug27_arr[i];
        }
        __sync_synchronize();
        _bug27_pred = 1;
    }
    return trigger;
}

static void _bug27_pattern_clean(void);
static int _bug27_pattern(int part);
static racebench_bug_helper _bug27_bug_helper;
static racebench_input_helper _bug27_input_helper;
__attribute__((constructor))
static void _bug27_input_helper_init(void)
{
    static const uint32_t indices[] = {0x37};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf05f7148, 0x1f22cd03};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1138b0e};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0xb095c959;
}
static int _bug27_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[27], 1);
        _bug27_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug27_pattern(part);
}
static int _bug27_input_check(int part)
{
    return rb_input_check(&_bug27_input_helper, 27, part, _bug27_bug_callback);
}
void racebench_bug27(int part)
{
    rb_bug_synchronize(&_bug27_bug_helper, 27, part, _bug27_input_check);
}

static void _bug28_pattern_clean(void);
static int _bug28_pattern(int part);
static pthread_mutex_t _bug28_mutex1;
static volatile __attribute__((used)) int _bug28_first_part;
static volatile __attribute__((used)) int _bug28_released;
static void _bug28_pattern_clean(void)
{
    pthread_mutex_init(&_bug28_mutex1, NULL);
    _bug28_first_part = -1;
    _bug28_released = -1;
}
static __attribute__((optimize(0))) int _bug28_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug28_mutex1) == 0)
                break;
            if (_bug28_released == 0) {
                _bug28_first_part = -1;
                _bug28_released = -1;
                return 1;
            }
        }
        if (_bug28_first_part == -1)
            _bug28_first_part = part;
        else {
            _bug28_first_part = -1;
            while (_bug28_released == -1) {}
            _bug28_released = -1;
        }
        pthread_mutex_unlock(&_bug28_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug28_mutex1);
        if (_bug28_first_part == -1) {
            _bug28_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug28_mutex1);
                _bug28_released = 1;
            }
            else {
                _bug28_released = 0;
                while (_bug28_released != -1) {}
                pthread_mutex_unlock(&_bug28_mutex1);
                return 1;
            }
        }
        else {
            _bug28_first_part = -1;
            pthread_mutex_unlock(&_bug28_mutex1);
        }
    }
    return 0;
}

static void _bug28_pattern_clean(void);
static int _bug28_pattern(int part);
static racebench_bug_helper _bug28_bug_helper;
static racebench_input_helper _bug28_input_helper;
__attribute__((constructor))
static void _bug28_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182dae};
    static racebench_before_helper before_helpers[1] = {};
    _bug28_input_helper.interleave_num = 1;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa1182dae;
}
static int _bug28_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[28], 1);
        _bug28_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug28_pattern(part);
}
static int _bug28_input_check(int part)
{
    return rb_input_check(&_bug28_input_helper, 28, part, _bug28_bug_callback);
}
void racebench_bug28(int part)
{
    rb_bug_synchronize(&_bug28_bug_helper, 28, part, _bug28_input_check);
}

static void _bug29_pattern_clean(void);
static int _bug29_pattern(int part);
static pthread_mutex_t _bug29_mutex1;
static volatile __attribute__((used)) int* volatile _bug29_ptr;
static volatile __attribute__((used)) int _bug29_val;
static void _bug29_pattern_clean(void)
{
    pthread_mutex_init(&_bug29_mutex1, NULL);
    _bug29_ptr = NULL;
    _bug29_val = 0;
}
static __attribute__((optimize(0))) int _bug29_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug29_mutex1);
        _bug29_ptr = &_bug29_val;
        pthread_mutex_unlock(&_bug29_mutex1);
        pthread_mutex_lock(&_bug29_mutex1);
        if (_bug29_ptr == NULL)
            trigger = 1;
        _bug29_ptr = NULL;
        pthread_mutex_unlock(&_bug29_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug29_mutex1);
        _bug29_ptr = &_bug29_val;
        if (_bug29_ptr == NULL)
            trigger = 1;
        _bug29_ptr = NULL;
        pthread_mutex_unlock(&_bug29_mutex1);
    }
    return trigger;
}

static void _bug29_pattern_clean(void);
static int _bug29_pattern(int part);
static racebench_bug_helper _bug29_bug_helper;
static racebench_input_helper _bug29_input_helper;
__attribute__((constructor))
static void _bug29_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1eb};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x40eaefcd, 0x75db73e4};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182f9e};
    static racebench_before_helper before_helpers[3] = {};
    _bug29_input_helper.interleave_num = 3;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x57de934f;
}
static int _bug29_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[29], 1);
        _bug29_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug29_pattern(part);
}
static int _bug29_input_check(int part)
{
    return rb_input_check(&_bug29_input_helper, 29, part, _bug29_bug_callback);
}
void racebench_bug29(int part)
{
    rb_bug_synchronize(&_bug29_bug_helper, 29, part, _bug29_input_check);
}

static void _bug30_pattern_clean(void);
static int _bug30_pattern(int part);
static pthread_mutex_t _bug30_mutex1;
static volatile __attribute__((used)) int* volatile _bug30_ptr;
static volatile __attribute__((used)) int _bug30_val;
static void _bug30_pattern_clean(void)
{
    pthread_mutex_init(&_bug30_mutex1, NULL);
    _bug30_ptr = NULL;
    _bug30_val = 0;
}
static __attribute__((optimize(0))) int _bug30_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug30_mutex1);
        _bug30_ptr = &_bug30_val;
        pthread_mutex_unlock(&_bug30_mutex1);
        pthread_mutex_lock(&_bug30_mutex1);
        if (_bug30_ptr == NULL)
            trigger = 1;
        _bug30_ptr = NULL;
        pthread_mutex_unlock(&_bug30_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug30_mutex1);
        _bug30_ptr = &_bug30_val;
        if (_bug30_ptr == NULL)
            trigger = 1;
        _bug30_ptr = NULL;
        pthread_mutex_unlock(&_bug30_mutex1);
    }
    return trigger;
}

static void _bug30_pattern_clean(void);
static int _bug30_pattern(int part);
static racebench_bug_helper _bug30_bug_helper;
static racebench_input_helper _bug30_input_helper;
__attribute__((constructor))
static void _bug30_input_helper_init(void)
{
    static const uint32_t indices[] = {0xa8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x6e3ca607, 0xb460cb83};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x9ab56a46};
    static racebench_before_helper before_helpers[3] = {};
    _bug30_input_helper.interleave_num = 3;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0xbd52dbd0;
}
static int _bug30_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[30], 1);
        _bug30_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug30_pattern(part);
}
static int _bug30_input_check(int part)
{
    return rb_input_check(&_bug30_input_helper, 30, part, _bug30_bug_callback);
}
void racebench_bug30(int part)
{
    rb_bug_synchronize(&_bug30_bug_helper, 30, part, _bug30_input_check);
}

static void _bug31_pattern_clean(void);
static int _bug31_pattern(int part);
static pthread_mutex_t _bug31_mutex1;
static volatile __attribute__((used)) int _bug31_first_part;
static volatile __attribute__((used)) int _bug31_released;
static void _bug31_pattern_clean(void)
{
    pthread_mutex_init(&_bug31_mutex1, NULL);
    _bug31_first_part = -1;
    _bug31_released = -1;
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug31_mutex1) == 0)
                break;
            if (_bug31_released == 0) {
                _bug31_first_part = -1;
                _bug31_released = -1;
                return 1;
            }
        }
        if (_bug31_first_part == -1)
            _bug31_first_part = part;
        else {
            _bug31_first_part = -1;
            while (_bug31_released == -1) {}
            _bug31_released = -1;
        }
        pthread_mutex_unlock(&_bug31_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug31_mutex1);
        if (_bug31_first_part == -1) {
            _bug31_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug31_mutex1);
                _bug31_released = 1;
            }
            else {
                _bug31_released = 0;
                while (_bug31_released != -1) {}
                pthread_mutex_unlock(&_bug31_mutex1);
                return 1;
            }
        }
        else {
            _bug31_first_part = -1;
            pthread_mutex_unlock(&_bug31_mutex1);
        }
    }
    return 0;
}

static void _bug31_pattern_clean(void);
static int _bug31_pattern(int part);
static racebench_bug_helper _bug31_bug_helper;
static racebench_input_helper _bug31_input_helper;
__attribute__((constructor))
static void _bug31_input_helper_init(void)
{
    static const uint32_t indices[] = {0x3b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xe6d6dc48};
    static racebench_before_helper before_helpers[1] = {};
    _bug31_input_helper.interleave_num = 1;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0xe6d6dc48;
}
static int _bug31_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[31], 1);
        _bug31_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug31_pattern(part);
}
static int _bug31_input_check(int part)
{
    return rb_input_check(&_bug31_input_helper, 31, part, _bug31_bug_callback);
}
void racebench_bug31(int part)
{
    rb_bug_synchronize(&_bug31_bug_helper, 31, part, _bug31_input_check);
}

static void _bug32_pattern_clean(void);
static int _bug32_pattern(int part);
static pthread_mutex_t _bug32_mutex1;
static volatile __attribute__((used)) int _bug32_first_part;
static volatile __attribute__((used)) int _bug32_released;
static void _bug32_pattern_clean(void)
{
    pthread_mutex_init(&_bug32_mutex1, NULL);
    _bug32_first_part = -1;
    _bug32_released = -1;
}
static __attribute__((optimize(0))) int _bug32_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug32_mutex1) == 0)
                break;
            if (_bug32_released == 0) {
                _bug32_first_part = -1;
                _bug32_released = -1;
                return 1;
            }
        }
        if (_bug32_first_part == -1)
            _bug32_first_part = part;
        else {
            _bug32_first_part = -1;
            while (_bug32_released == -1) {}
            _bug32_released = -1;
        }
        pthread_mutex_unlock(&_bug32_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug32_mutex1);
        if (_bug32_first_part == -1) {
            _bug32_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug32_mutex1);
                _bug32_released = 1;
            }
            else {
                _bug32_released = 0;
                while (_bug32_released != -1) {}
                pthread_mutex_unlock(&_bug32_mutex1);
                return 1;
            }
        }
        else {
            _bug32_first_part = -1;
            pthread_mutex_unlock(&_bug32_mutex1);
        }
    }
    return 0;
}

static void _bug32_pattern_clean(void);
static int _bug32_pattern(int part);
static racebench_bug_helper _bug32_bug_helper;
static racebench_input_helper _bug32_input_helper;
__attribute__((constructor))
static void _bug32_input_helper_init(void)
{
    static const uint32_t indices[] = {0x229};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x29dfc3, 0xe2000bbc};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1189cde};
    static racebench_before_helper before_helpers[3] = {};
    _bug32_input_helper.interleave_num = 3;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0x8342885d;
}
static int _bug32_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[32], 1);
        _bug32_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug32_pattern(part);
}
static int _bug32_input_check(int part)
{
    return rb_input_check(&_bug32_input_helper, 32, part, _bug32_bug_callback);
}
void racebench_bug32(int part)
{
    rb_bug_synchronize(&_bug32_bug_helper, 32, part, _bug32_input_check);
}

static void _bug33_pattern_clean(void);
static int _bug33_pattern(int part);
static volatile __attribute__((used)) int _bug33_ok;
static volatile __attribute__((used)) int* volatile _bug33_ptr;
static volatile __attribute__((used)) int _bug33_val;
static void _bug33_pattern_clean(void)
{
    _bug33_ok = 0;
    _bug33_ptr = NULL;
    _bug33_val = 0;
}
static __attribute__((optimize(0))) int _bug33_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug33_ok = 1;
        __sync_synchronize();
        _bug33_ptr = &_bug33_val;
    }
    else {
        while (!_bug33_ok)
            pthread_yield();
        if (_bug33_ptr == NULL)
            trigger = 1;
        _bug33_ptr = NULL;
        _bug33_ok = 0;
    }
    return trigger;
}

static void _bug33_pattern_clean(void);
static int _bug33_pattern(int part);
static racebench_bug_helper _bug33_bug_helper;
static racebench_input_helper _bug33_input_helper;
__attribute__((constructor))
static void _bug33_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x42f8a561};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug33_input_helper.interleave_num = 2;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0xe410d21f;
}
static int _bug33_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[33], 1);
        _bug33_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug33_pattern(part);
}
static int _bug33_input_check(int part)
{
    return rb_input_check(&_bug33_input_helper, 33, part, _bug33_bug_callback);
}
void racebench_bug33(int part)
{
    rb_bug_synchronize(&_bug33_bug_helper, 33, part, _bug33_input_check);
}

static void _bug34_pattern_clean(void);
static int _bug34_pattern(int part);
static volatile __attribute__((used)) int _bug34_ok;
static volatile __attribute__((used)) int* volatile _bug34_ptr;
static volatile __attribute__((used)) int _bug34_val;
static void _bug34_pattern_clean(void)
{
    _bug34_ok = 0;
    _bug34_ptr = NULL;
    _bug34_val = 0;
}
static __attribute__((optimize(0))) int _bug34_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug34_ok = 1;
        __sync_synchronize();
        _bug34_ptr = &_bug34_val;
    }
    else {
        while (!_bug34_ok)
            pthread_yield();
        if (_bug34_ptr == NULL)
            trigger = 1;
        _bug34_ptr = NULL;
        _bug34_ok = 0;
    }
    return trigger;
}

static void _bug34_pattern_clean(void);
static int _bug34_pattern(int part);
static racebench_bug_helper _bug34_bug_helper;
static racebench_input_helper _bug34_input_helper;
__attribute__((constructor))
static void _bug34_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ec};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x268cf938};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x27151a31};
    static racebench_before_helper before_helpers[2] = {};
    _bug34_input_helper.interleave_num = 2;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0x4da21369;
}
static int _bug34_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[34], 1);
        _bug34_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug34_pattern(part);
}
static int _bug34_input_check(int part)
{
    return rb_input_check(&_bug34_input_helper, 34, part, _bug34_bug_callback);
}
void racebench_bug34(int part)
{
    rb_bug_synchronize(&_bug34_bug_helper, 34, part, _bug34_input_check);
}

static void _bug35_pattern_clean(void);
static int _bug35_pattern(int part);
static volatile __attribute__((used)) int _bug35_arr[3];
static volatile __attribute__((used)) int _bug35_val;
static volatile __attribute__((used)) int _bug35_pred;
static void _bug35_pattern_clean(void)
{
    _bug35_arr[0] = 0, _bug35_arr[1] = 2, _bug35_arr[2] = 1;
    _bug35_val = 0;
    _bug35_pred = 0;
}
static __attribute__((optimize(0))) int _bug35_pattern(int part)
{
    int trigger = 0;
    _bug35_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug35_val += _bug35_arr[i];
        }
        __sync_synchronize();
        while (!_bug35_pred)
            pthread_yield();
        trigger = _bug35_val != 3;
        _bug35_pred = 0;
        _bug35_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug35_val += _bug35_arr[i];
        }
        __sync_synchronize();
        _bug35_pred = 1;
    }
    return trigger;
}

static void _bug35_pattern_clean(void);
static int _bug35_pattern(int part);
static racebench_bug_helper _bug35_bug_helper;
static racebench_input_helper _bug35_input_helper;
__attribute__((constructor))
static void _bug35_input_helper_init(void)
{
    static const uint32_t indices[] = {0x167};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb07a0455, 0x172c8af8};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182bee};
    static racebench_before_helper before_helpers[3] = {};
    _bug35_input_helper.interleave_num = 3;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0x68bebb3b;
}
static int _bug35_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[35], 1);
        _bug35_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug35_pattern(part);
}
static int _bug35_input_check(int part)
{
    return rb_input_check(&_bug35_input_helper, 35, part, _bug35_bug_callback);
}
void racebench_bug35(int part)
{
    rb_bug_synchronize(&_bug35_bug_helper, 35, part, _bug35_input_check);
}

static void _bug36_pattern_clean(void);
static int _bug36_pattern(int part);
static pthread_mutex_t _bug36_mutex1;
static volatile __attribute__((used)) int _bug36_first_part;
static volatile __attribute__((used)) int _bug36_released;
static void _bug36_pattern_clean(void)
{
    pthread_mutex_init(&_bug36_mutex1, NULL);
    _bug36_first_part = -1;
    _bug36_released = -1;
}
static __attribute__((optimize(0))) int _bug36_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug36_mutex1) == 0)
                break;
            if (_bug36_released == 0) {
                _bug36_first_part = -1;
                _bug36_released = -1;
                return 1;
            }
        }
        if (_bug36_first_part == -1)
            _bug36_first_part = part;
        else {
            _bug36_first_part = -1;
            while (_bug36_released == -1) {}
            _bug36_released = -1;
        }
        pthread_mutex_unlock(&_bug36_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug36_mutex1);
        if (_bug36_first_part == -1) {
            _bug36_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug36_mutex1);
                _bug36_released = 1;
            }
            else {
                _bug36_released = 0;
                while (_bug36_released != -1) {}
                pthread_mutex_unlock(&_bug36_mutex1);
                return 1;
            }
        }
        else {
            _bug36_first_part = -1;
            pthread_mutex_unlock(&_bug36_mutex1);
        }
    }
    return 0;
}

static void _bug36_pattern_clean(void);
static int _bug36_pattern(int part);
static racebench_bug_helper _bug36_bug_helper;
static racebench_input_helper _bug36_input_helper;
__attribute__((constructor))
static void _bug36_input_helper_init(void)
{
    static const uint32_t indices[] = {0x22e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf24826d3, 0x8fe6bfea};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa116713e};
    static racebench_before_helper before_helpers[3] = {};
    _bug36_input_helper.interleave_num = 3;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0x234557fb;
}
static int _bug36_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[36], 1);
        _bug36_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug36_pattern(part);
}
static int _bug36_input_check(int part)
{
    return rb_input_check(&_bug36_input_helper, 36, part, _bug36_bug_callback);
}
void racebench_bug36(int part)
{
    rb_bug_synchronize(&_bug36_bug_helper, 36, part, _bug36_input_check);
}

static void _bug37_pattern_clean(void);
static int _bug37_pattern(int part);
static volatile __attribute__((used)) int _bug37_arr[3];
static volatile __attribute__((used)) int _bug37_val;
static volatile __attribute__((used)) int _bug37_pred;
static void _bug37_pattern_clean(void)
{
    _bug37_arr[0] = 0, _bug37_arr[1] = 2, _bug37_arr[2] = 1;
    _bug37_val = 0;
    _bug37_pred = 0;
}
static __attribute__((optimize(0))) int _bug37_pattern(int part)
{
    int trigger = 0;
    _bug37_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug37_val += _bug37_arr[i];
        }
        __sync_synchronize();
        while (!_bug37_pred)
            pthread_yield();
        trigger = _bug37_val != 3;
        _bug37_pred = 0;
        _bug37_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug37_val += _bug37_arr[i];
        }
        __sync_synchronize();
        _bug37_pred = 1;
    }
    return trigger;
}

static void _bug37_pattern_clean(void);
static int _bug37_pattern(int part);
static racebench_bug_helper _bug37_bug_helper;
static racebench_input_helper _bug37_input_helper;
__attribute__((constructor))
static void _bug37_input_helper_init(void)
{
    static const uint32_t indices[] = {0x11};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1119fce};
    static racebench_before_helper before_helpers[1] = {};
    _bug37_input_helper.interleave_num = 1;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0xa1119fce;
}
static int _bug37_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[37], 1);
        _bug37_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug37_pattern(part);
}
static int _bug37_input_check(int part)
{
    return rb_input_check(&_bug37_input_helper, 37, part, _bug37_bug_callback);
}
void racebench_bug37(int part)
{
    rb_bug_synchronize(&_bug37_bug_helper, 37, part, _bug37_input_check);
}

static void _bug38_pattern_clean(void);
static int _bug38_pattern(int part);
static volatile __attribute__((used)) int _bug38_ok;
static volatile __attribute__((used)) int* volatile _bug38_ptr;
static volatile __attribute__((used)) int _bug38_val;
static void _bug38_pattern_clean(void)
{
    _bug38_ok = 0;
    _bug38_ptr = NULL;
    _bug38_val = 0;
}
static __attribute__((optimize(0))) int _bug38_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug38_ok = 1;
        __sync_synchronize();
        _bug38_ptr = &_bug38_val;
    }
    else {
        while (!_bug38_ok)
            pthread_yield();
        if (_bug38_ptr == NULL)
            trigger = 1;
        _bug38_ptr = NULL;
        _bug38_ok = 0;
    }
    return trigger;
}

static void _bug38_pattern_clean(void);
static int _bug38_pattern(int part);
static racebench_bug_helper _bug38_bug_helper;
static racebench_input_helper _bug38_input_helper;
__attribute__((constructor))
static void _bug38_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1189c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug38_input_helper.interleave_num = 0;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xa1189c4e;
}
static int _bug38_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[38], 1);
        _bug38_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug38_pattern(part);
}
static int _bug38_input_check(int part)
{
    return rb_input_check(&_bug38_input_helper, 38, part, _bug38_bug_callback);
}
void racebench_bug38(int part)
{
    rb_bug_synchronize(&_bug38_bug_helper, 38, part, _bug38_input_check);
}

static void _bug39_pattern_clean(void);
static int _bug39_pattern(int part);
static pthread_mutex_t _bug39_mutex1;
static pthread_mutex_t _bug39_mutex2;
static void _bug39_pattern_clean(void)
{
    pthread_mutex_init(&_bug39_mutex1, NULL);
    pthread_mutex_init(&_bug39_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug39_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug39_mutex1);
        if (pthread_mutex_trylock(&_bug39_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug39_mutex2);
        pthread_mutex_unlock(&_bug39_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug39_mutex2);
        if (pthread_mutex_trylock(&_bug39_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug39_mutex1);
        pthread_mutex_unlock(&_bug39_mutex2);
    }
    return trigger;
}

static void _bug39_pattern_clean(void);
static int _bug39_pattern(int part);
static racebench_bug_helper _bug39_bug_helper;
static racebench_input_helper _bug39_input_helper;
__attribute__((constructor))
static void _bug39_input_helper_init(void)
{
    static const uint32_t indices[] = {0x227};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f5e};
    static racebench_before_helper before_helpers[1] = {};
    _bug39_input_helper.interleave_num = 1;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0xa1182f5e;
}
static int _bug39_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[39], 1);
        _bug39_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug39_pattern(part);
}
static int _bug39_input_check(int part)
{
    return rb_input_check(&_bug39_input_helper, 39, part, _bug39_bug_callback);
}
void racebench_bug39(int part)
{
    rb_bug_synchronize(&_bug39_bug_helper, 39, part, _bug39_input_check);
}

static void _bug40_pattern_clean(void);
static int _bug40_pattern(int part);
static pthread_mutex_t _bug40_mutex1;
static pthread_cond_t _bug40_cond1;
static volatile __attribute__((used)) int* volatile _bug40_ptr;
static volatile __attribute__((used)) int _bug40_val;
static volatile __attribute__((used)) int _bug40_pred;
static void _bug40_pattern_clean(void)
{
    pthread_mutex_init(&_bug40_mutex1, NULL);
    pthread_cond_init(&_bug40_cond1, NULL);
    _bug40_ptr = NULL;
    _bug40_val = 0;
    _bug40_pred = 0;
}
static __attribute__((optimize(0))) int _bug40_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug40_pred = 0;
        pthread_cond_signal(&_bug40_cond1);
        pthread_mutex_lock(&_bug40_mutex1);
        _bug40_ptr = &_bug40_val;
        _bug40_pred = 1;
        pthread_mutex_unlock(&_bug40_mutex1);
        pthread_cond_signal(&_bug40_cond1);
    }
    else {
        pthread_mutex_lock(&_bug40_mutex1);
        if (!_bug40_pred)
            pthread_cond_wait(&_bug40_cond1, &_bug40_mutex1);
        if (_bug40_ptr == NULL)
            trigger = 1;
        _bug40_ptr = NULL;
        _bug40_pred = 0;
        pthread_mutex_unlock(&_bug40_mutex1);
    }
    return trigger;
}

static void _bug40_pattern_clean(void);
static int _bug40_pattern(int part);
static racebench_bug_helper _bug40_bug_helper;
static racebench_input_helper _bug40_input_helper;
__attribute__((constructor))
static void _bug40_input_helper_init(void)
{
    static const uint32_t indices[] = {0x7f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xa5fd9e26, 0x7c72ff67};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118294e};
    static racebench_before_helper before_helpers[3] = {};
    _bug40_input_helper.interleave_num = 3;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xc388c6db;
}
static int _bug40_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[40], 1);
        _bug40_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug40_pattern(part);
}
static int _bug40_input_check(int part)
{
    return rb_input_check(&_bug40_input_helper, 40, part, _bug40_bug_callback);
}
void racebench_bug40(int part)
{
    rb_bug_synchronize(&_bug40_bug_helper, 40, part, _bug40_input_check);
}

static void _bug41_pattern_clean(void);
static int _bug41_pattern(int part);
static volatile __attribute__((used)) int _bug41_arr[3];
static volatile __attribute__((used)) int _bug41_val;
static volatile __attribute__((used)) int _bug41_pred;
static void _bug41_pattern_clean(void)
{
    _bug41_arr[0] = 0, _bug41_arr[1] = 2, _bug41_arr[2] = 1;
    _bug41_val = 0;
    _bug41_pred = 0;
}
static __attribute__((optimize(0))) int _bug41_pattern(int part)
{
    int trigger = 0;
    _bug41_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug41_val += _bug41_arr[i];
        }
        __sync_synchronize();
        while (!_bug41_pred)
            pthread_yield();
        trigger = _bug41_val != 3;
        _bug41_pred = 0;
        _bug41_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug41_val += _bug41_arr[i];
        }
        __sync_synchronize();
        _bug41_pred = 1;
    }
    return trigger;
}

static void _bug41_pattern_clean(void);
static int _bug41_pattern(int part);
static racebench_bug_helper _bug41_bug_helper;
static racebench_input_helper _bug41_input_helper;
__attribute__((constructor))
static void _bug41_input_helper_init(void)
{
    static const uint32_t indices[] = {0x12};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa112ecce};
    static racebench_before_helper before_helpers[1] = {};
    _bug41_input_helper.interleave_num = 1;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa112ecce;
}
static int _bug41_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[41], 1);
        _bug41_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug41_pattern(part);
}
static int _bug41_input_check(int part)
{
    return rb_input_check(&_bug41_input_helper, 41, part, _bug41_bug_callback);
}
void racebench_bug41(int part)
{
    rb_bug_synchronize(&_bug41_bug_helper, 41, part, _bug41_input_check);
}

static void _bug42_pattern_clean(void);
static int _bug42_pattern(int part);
static pthread_mutex_t _bug42_mutex1;
static pthread_mutex_t _bug42_mutex2;
static void _bug42_pattern_clean(void)
{
    pthread_mutex_init(&_bug42_mutex1, NULL);
    pthread_mutex_init(&_bug42_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug42_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug42_mutex1);
        if (pthread_mutex_trylock(&_bug42_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug42_mutex2);
        pthread_mutex_unlock(&_bug42_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug42_mutex2);
        if (pthread_mutex_trylock(&_bug42_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug42_mutex1);
        pthread_mutex_unlock(&_bug42_mutex2);
    }
    return trigger;
}

static void _bug42_pattern_clean(void);
static int _bug42_pattern(int part);
static racebench_bug_helper _bug42_bug_helper;
static racebench_input_helper _bug42_input_helper;
__attribute__((constructor))
static void _bug42_input_helper_init(void)
{
    static const uint32_t indices[] = {0x203};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x48555049};
    static racebench_before_helper before_helpers[0] = {};
    _bug42_input_helper.interleave_num = 0;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0x48555049;
}
static int _bug42_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[42], 1);
        _bug42_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug42_pattern(part);
}
static int _bug42_input_check(int part)
{
    return rb_input_check(&_bug42_input_helper, 42, part, _bug42_bug_callback);
}
void racebench_bug42(int part)
{
    rb_bug_synchronize(&_bug42_bug_helper, 42, part, _bug42_input_check);
}

static void _bug43_pattern_clean(void);
static int _bug43_pattern(int part);
static volatile __attribute__((used)) int _bug43_ok;
static volatile __attribute__((used)) int* volatile _bug43_ptr;
static volatile __attribute__((used)) int _bug43_val;
static void _bug43_pattern_clean(void)
{
    _bug43_ok = 0;
    _bug43_ptr = NULL;
    _bug43_val = 0;
}
static __attribute__((optimize(0))) int _bug43_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug43_ok = 1;
        __sync_synchronize();
        _bug43_ptr = &_bug43_val;
    }
    else {
        while (!_bug43_ok)
            pthread_yield();
        if (_bug43_ptr == NULL)
            trigger = 1;
        _bug43_ptr = NULL;
        _bug43_ok = 0;
    }
    return trigger;
}

static void _bug43_pattern_clean(void);
static int _bug43_pattern(int part);
static racebench_bug_helper _bug43_bug_helper;
static racebench_input_helper _bug43_input_helper;
__attribute__((constructor))
static void _bug43_input_helper_init(void)
{
    static const uint32_t indices[] = {0x8a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11d6cde};
    static racebench_before_helper before_helpers[1] = {};
    _bug43_input_helper.interleave_num = 1;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0xa11d6cde;
}
static int _bug43_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[43], 1);
        _bug43_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug43_pattern(part);
}
static int _bug43_input_check(int part)
{
    return rb_input_check(&_bug43_input_helper, 43, part, _bug43_bug_callback);
}
void racebench_bug43(int part)
{
    rb_bug_synchronize(&_bug43_bug_helper, 43, part, _bug43_input_check);
}

static void _bug44_pattern_clean(void);
static int _bug44_pattern(int part);
static pthread_mutex_t _bug44_mutex1;
static pthread_cond_t _bug44_cond1;
static volatile __attribute__((used)) int* volatile _bug44_ptr;
static volatile __attribute__((used)) int _bug44_val;
static volatile __attribute__((used)) int _bug44_pred;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    pthread_cond_init(&_bug44_cond1, NULL);
    _bug44_ptr = NULL;
    _bug44_val = 0;
    _bug44_pred = 0;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug44_pred = 0;
        pthread_cond_signal(&_bug44_cond1);
        pthread_mutex_lock(&_bug44_mutex1);
        _bug44_ptr = &_bug44_val;
        _bug44_pred = 1;
        pthread_mutex_unlock(&_bug44_mutex1);
        pthread_cond_signal(&_bug44_cond1);
    }
    else {
        pthread_mutex_lock(&_bug44_mutex1);
        if (!_bug44_pred)
            pthread_cond_wait(&_bug44_cond1, &_bug44_mutex1);
        if (_bug44_ptr == NULL)
            trigger = 1;
        _bug44_ptr = NULL;
        _bug44_pred = 0;
        pthread_mutex_unlock(&_bug44_mutex1);
    }
    return trigger;
}

static void _bug44_pattern_clean(void);
static int _bug44_pattern(int part);
static racebench_bug_helper _bug44_bug_helper;
static racebench_input_helper _bug44_input_helper;
__attribute__((constructor))
static void _bug44_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x790322d, 0x189fff39};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118305e};
    static racebench_before_helper before_helpers[3] = {};
    _bug44_input_helper.interleave_num = 3;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xc14861c4;
}
static int _bug44_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[44], 1);
        _bug44_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug44_pattern(part);
}
static int _bug44_input_check(int part)
{
    return rb_input_check(&_bug44_input_helper, 44, part, _bug44_bug_callback);
}
void racebench_bug44(int part)
{
    rb_bug_synchronize(&_bug44_bug_helper, 44, part, _bug44_input_check);
}

static void _bug45_pattern_clean(void);
static int _bug45_pattern(int part);
static pthread_mutex_t _bug45_mutex1;
static volatile __attribute__((used)) int* volatile _bug45_ptr;
static volatile __attribute__((used)) int _bug45_val;
static void _bug45_pattern_clean(void)
{
    pthread_mutex_init(&_bug45_mutex1, NULL);
    _bug45_ptr = NULL;
    _bug45_val = 0;
}
static __attribute__((optimize(0))) int _bug45_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug45_mutex1);
        _bug45_ptr = &_bug45_val;
        pthread_mutex_unlock(&_bug45_mutex1);
        pthread_mutex_lock(&_bug45_mutex1);
        if (_bug45_ptr == NULL)
            trigger = 1;
        _bug45_ptr = NULL;
        pthread_mutex_unlock(&_bug45_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug45_mutex1);
        _bug45_ptr = &_bug45_val;
        if (_bug45_ptr == NULL)
            trigger = 1;
        _bug45_ptr = NULL;
        pthread_mutex_unlock(&_bug45_mutex1);
    }
    return trigger;
}

static void _bug45_pattern_clean(void);
static int _bug45_pattern(int part);
static racebench_bug_helper _bug45_bug_helper;
static racebench_input_helper _bug45_input_helper;
__attribute__((constructor))
static void _bug45_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ec};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x94e2831};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xf825ac1f};
    static racebench_before_helper before_helpers[2] = {};
    _bug45_input_helper.interleave_num = 2;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0x173d450;
}
static int _bug45_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[45], 1);
        _bug45_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug45_pattern(part);
}
static int _bug45_input_check(int part)
{
    return rb_input_check(&_bug45_input_helper, 45, part, _bug45_bug_callback);
}
void racebench_bug45(int part)
{
    rb_bug_synchronize(&_bug45_bug_helper, 45, part, _bug45_input_check);
}

static void _bug46_pattern_clean(void);
static int _bug46_pattern(int part);
static pthread_mutex_t _bug46_mutex1;
static volatile __attribute__((used)) int* volatile _bug46_ptr;
static volatile __attribute__((used)) int _bug46_val;
static void _bug46_pattern_clean(void)
{
    pthread_mutex_init(&_bug46_mutex1, NULL);
    _bug46_ptr = NULL;
    _bug46_val = 0;
}
static __attribute__((optimize(0))) int _bug46_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug46_mutex1);
        _bug46_ptr = &_bug46_val;
        pthread_mutex_unlock(&_bug46_mutex1);
        pthread_mutex_lock(&_bug46_mutex1);
        if (_bug46_ptr == NULL)
            trigger = 1;
        _bug46_ptr = NULL;
        pthread_mutex_unlock(&_bug46_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug46_mutex1);
        _bug46_ptr = &_bug46_val;
        if (_bug46_ptr == NULL)
            trigger = 1;
        _bug46_ptr = NULL;
        pthread_mutex_unlock(&_bug46_mutex1);
    }
    return trigger;
}

static void _bug46_pattern_clean(void);
static int _bug46_pattern(int part);
static racebench_bug_helper _bug46_bug_helper;
static racebench_input_helper _bug46_input_helper;
__attribute__((constructor))
static void _bug46_input_helper_init(void)
{
    static const uint32_t indices[] = {0x21e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x4c365004};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118309e};
    static racebench_before_helper before_helpers[2] = {};
    _bug46_input_helper.interleave_num = 2;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0xed4e80a2;
}
static int _bug46_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[46], 1);
        _bug46_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug46_pattern(part);
}
static int _bug46_input_check(int part)
{
    return rb_input_check(&_bug46_input_helper, 46, part, _bug46_bug_callback);
}
void racebench_bug46(int part)
{
    rb_bug_synchronize(&_bug46_bug_helper, 46, part, _bug46_input_check);
}

static void _bug47_pattern_clean(void);
static int _bug47_pattern(int part);
static pthread_mutex_t _bug47_mutex1;
static volatile __attribute__((used)) int* volatile _bug47_ptr;
static volatile __attribute__((used)) int _bug47_val;
static void _bug47_pattern_clean(void)
{
    pthread_mutex_init(&_bug47_mutex1, NULL);
    _bug47_ptr = NULL;
    _bug47_val = 0;
}
static __attribute__((optimize(0))) int _bug47_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug47_mutex1);
        _bug47_ptr = &_bug47_val;
        pthread_mutex_unlock(&_bug47_mutex1);
        pthread_mutex_lock(&_bug47_mutex1);
        if (_bug47_ptr == NULL)
            trigger = 1;
        _bug47_ptr = NULL;
        pthread_mutex_unlock(&_bug47_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug47_mutex1);
        _bug47_ptr = &_bug47_val;
        if (_bug47_ptr == NULL)
            trigger = 1;
        _bug47_ptr = NULL;
        pthread_mutex_unlock(&_bug47_mutex1);
    }
    return trigger;
}

static void _bug47_pattern_clean(void);
static int _bug47_pattern(int part);
static racebench_bug_helper _bug47_bug_helper;
static racebench_input_helper _bug47_input_helper;
__attribute__((constructor))
static void _bug47_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1112bae};
    static racebench_before_helper before_helpers[1] = {};
    _bug47_input_helper.interleave_num = 1;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0xa1112bae;
}
static int _bug47_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[47], 1);
        _bug47_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug47_pattern(part);
}
static int _bug47_input_check(int part)
{
    return rb_input_check(&_bug47_input_helper, 47, part, _bug47_bug_callback);
}
void racebench_bug47(int part)
{
    rb_bug_synchronize(&_bug47_bug_helper, 47, part, _bug47_input_check);
}

static void _bug48_pattern_clean(void);
static int _bug48_pattern(int part);
static volatile __attribute__((used)) int _bug48_arr[3];
static volatile __attribute__((used)) int _bug48_val;
static volatile __attribute__((used)) int _bug48_pred;
static void _bug48_pattern_clean(void)
{
    _bug48_arr[0] = 0, _bug48_arr[1] = 2, _bug48_arr[2] = 1;
    _bug48_val = 0;
    _bug48_pred = 0;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    int trigger = 0;
    _bug48_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug48_val += _bug48_arr[i];
        }
        __sync_synchronize();
        while (!_bug48_pred)
            pthread_yield();
        trigger = _bug48_val != 3;
        _bug48_pred = 0;
        _bug48_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug48_val += _bug48_arr[i];
        }
        __sync_synchronize();
        _bug48_pred = 1;
    }
    return trigger;
}

static void _bug48_pattern_clean(void);
static int _bug48_pattern(int part);
static racebench_bug_helper _bug48_bug_helper;
static racebench_input_helper _bug48_input_helper;
__attribute__((constructor))
static void _bug48_input_helper_init(void)
{
    static const uint32_t indices[] = {0x20e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xc485ac54};
    static racebench_before_helper before_helpers[0] = {};
    _bug48_input_helper.interleave_num = 0;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0xc485ac54;
}
static int _bug48_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[48], 1);
        _bug48_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug48_pattern(part);
}
static int _bug48_input_check(int part)
{
    return rb_input_check(&_bug48_input_helper, 48, part, _bug48_bug_callback);
}
void racebench_bug48(int part)
{
    rb_bug_synchronize(&_bug48_bug_helper, 48, part, _bug48_input_check);
}

static void _bug49_pattern_clean(void);
static int _bug49_pattern(int part);
static pthread_mutex_t _bug49_mutex1;
static volatile __attribute__((used)) int _bug49_val;
static void _bug49_pattern_clean(void)
{
    pthread_mutex_init(&_bug49_mutex1, NULL);
    _bug49_val = 0;
}
static __attribute__((optimize(0))) int _bug49_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug49_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug49_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug49_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug49_pattern_clean(void);
static int _bug49_pattern(int part);
static racebench_bug_helper _bug49_bug_helper;
static racebench_input_helper _bug49_input_helper;
__attribute__((constructor))
static void _bug49_input_helper_init(void)
{
    static const uint32_t indices[] = {0x22c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xf3f58b9a};
    static racebench_before_helper before_helpers[1] = {};
    _bug49_input_helper.interleave_num = 1;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xf3f58b9a;
}
static int _bug49_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[49], 1);
        _bug49_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug49_pattern(part);
}
static int _bug49_input_check(int part)
{
    return rb_input_check(&_bug49_input_helper, 49, part, _bug49_bug_callback);
}
void racebench_bug49(int part)
{
    rb_bug_synchronize(&_bug49_bug_helper, 49, part, _bug49_input_check);
}

static void _bug50_pattern_clean(void);
static int _bug50_pattern(int part);
static volatile __attribute__((used)) int _bug50_arr[3];
static volatile __attribute__((used)) int _bug50_val;
static volatile __attribute__((used)) int _bug50_pred;
static void _bug50_pattern_clean(void)
{
    _bug50_arr[0] = 0, _bug50_arr[1] = 2, _bug50_arr[2] = 1;
    _bug50_val = 0;
    _bug50_pred = 0;
}
static __attribute__((optimize(0))) int _bug50_pattern(int part)
{
    int trigger = 0;
    _bug50_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug50_val += _bug50_arr[i];
        }
        __sync_synchronize();
        while (!_bug50_pred)
            pthread_yield();
        trigger = _bug50_val != 3;
        _bug50_pred = 0;
        _bug50_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug50_val += _bug50_arr[i];
        }
        __sync_synchronize();
        _bug50_pred = 1;
    }
    return trigger;
}

static void _bug50_pattern_clean(void);
static int _bug50_pattern(int part);
static racebench_bug_helper _bug50_bug_helper;
static racebench_input_helper _bug50_input_helper;
__attribute__((constructor))
static void _bug50_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1e8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xa3710f6a, 0x380fcece};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11571ce};
    static racebench_before_helper before_helpers[3] = {};
    _bug50_input_helper.interleave_num = 3;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0x7c965006;
}
static int _bug50_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[50], 1);
        _bug50_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug50_pattern(part);
}
static int _bug50_input_check(int part)
{
    return rb_input_check(&_bug50_input_helper, 50, part, _bug50_bug_callback);
}
void racebench_bug50(int part)
{
    rb_bug_synchronize(&_bug50_bug_helper, 50, part, _bug50_input_check);
}

static void _bug51_pattern_clean(void);
static int _bug51_pattern(int part);
static pthread_mutex_t _bug51_mutex1;
static volatile __attribute__((used)) int* volatile _bug51_ptr;
static volatile __attribute__((used)) int _bug51_val;
static void _bug51_pattern_clean(void)
{
    pthread_mutex_init(&_bug51_mutex1, NULL);
    _bug51_ptr = NULL;
    _bug51_val = 0;
}
static __attribute__((optimize(0))) int _bug51_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug51_mutex1);
        _bug51_ptr = &_bug51_val;
        pthread_mutex_unlock(&_bug51_mutex1);
        pthread_mutex_lock(&_bug51_mutex1);
        if (_bug51_ptr == NULL)
            trigger = 1;
        _bug51_ptr = NULL;
        pthread_mutex_unlock(&_bug51_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug51_mutex1);
        _bug51_ptr = &_bug51_val;
        if (_bug51_ptr == NULL)
            trigger = 1;
        _bug51_ptr = NULL;
        pthread_mutex_unlock(&_bug51_mutex1);
    }
    return trigger;
}

static void _bug51_pattern_clean(void);
static int _bug51_pattern(int part);
static racebench_bug_helper _bug51_bug_helper;
static racebench_input_helper _bug51_input_helper;
__attribute__((constructor))
static void _bug51_input_helper_init(void)
{
    static const uint32_t indices[] = {0x215};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182d1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug51_input_helper.interleave_num = 0;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0xa1182d1e;
}
static int _bug51_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[51], 1);
        _bug51_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug51_pattern(part);
}
static int _bug51_input_check(int part)
{
    return rb_input_check(&_bug51_input_helper, 51, part, _bug51_bug_callback);
}
void racebench_bug51(int part)
{
    rb_bug_synchronize(&_bug51_bug_helper, 51, part, _bug51_input_check);
}

static void _bug52_pattern_clean(void);
static int _bug52_pattern(int part);
static pthread_mutex_t _bug52_mutex1;
static volatile __attribute__((used)) int _bug52_val;
static void _bug52_pattern_clean(void)
{
    pthread_mutex_init(&_bug52_mutex1, NULL);
    _bug52_val = 0;
}
static __attribute__((optimize(0))) int _bug52_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug52_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug52_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug52_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug52_pattern_clean(void);
static int _bug52_pattern(int part);
static racebench_bug_helper _bug52_bug_helper;
static racebench_input_helper _bug52_input_helper;
__attribute__((constructor))
static void _bug52_input_helper_init(void)
{
    static const uint32_t indices[] = {0x210};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xc29b1c27, 0xf87effa6};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11830ee};
    static racebench_before_helper before_helpers[3] = {};
    _bug52_input_helper.interleave_num = 3;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0x5c324cbb;
}
static int _bug52_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[52], 1);
        _bug52_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug52_pattern(part);
}
static int _bug52_input_check(int part)
{
    return rb_input_check(&_bug52_input_helper, 52, part, _bug52_bug_callback);
}
void racebench_bug52(int part)
{
    rb_bug_synchronize(&_bug52_bug_helper, 52, part, _bug52_input_check);
}

static void _bug53_pattern_clean(void);
static int _bug53_pattern(int part);
static volatile __attribute__((used)) int _bug53_arr[3];
static volatile __attribute__((used)) int _bug53_val;
static volatile __attribute__((used)) int _bug53_pred;
static void _bug53_pattern_clean(void)
{
    _bug53_arr[0] = 0, _bug53_arr[1] = 2, _bug53_arr[2] = 1;
    _bug53_val = 0;
    _bug53_pred = 0;
}
static __attribute__((optimize(0))) int _bug53_pattern(int part)
{
    int trigger = 0;
    _bug53_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug53_val += _bug53_arr[i];
        }
        __sync_synchronize();
        while (!_bug53_pred)
            pthread_yield();
        trigger = _bug53_val != 3;
        _bug53_pred = 0;
        _bug53_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug53_val += _bug53_arr[i];
        }
        __sync_synchronize();
        _bug53_pred = 1;
    }
    return trigger;
}

static void _bug53_pattern_clean(void);
static int _bug53_pattern(int part);
static racebench_bug_helper _bug53_bug_helper;
static racebench_input_helper _bug53_input_helper;
__attribute__((constructor))
static void _bug53_input_helper_init(void)
{
    static const uint32_t indices[] = {0x202};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11f8b3e};
    static racebench_before_helper before_helpers[0] = {};
    _bug53_input_helper.interleave_num = 0;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0xa11f8b3e;
}
static int _bug53_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[53], 1);
        _bug53_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug53_pattern(part);
}
static int _bug53_input_check(int part)
{
    return rb_input_check(&_bug53_input_helper, 53, part, _bug53_bug_callback);
}
void racebench_bug53(int part)
{
    rb_bug_synchronize(&_bug53_bug_helper, 53, part, _bug53_input_check);
}

static void _bug54_pattern_clean(void);
static int _bug54_pattern(int part);
static pthread_mutex_t _bug54_mutex1;
static volatile __attribute__((used)) int _bug54_val;
static void _bug54_pattern_clean(void)
{
    pthread_mutex_init(&_bug54_mutex1, NULL);
    _bug54_val = 0;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug54_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug54_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug54_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug54_pattern_clean(void);
static int _bug54_pattern(int part);
static racebench_bug_helper _bug54_bug_helper;
static racebench_input_helper _bug54_input_helper;
__attribute__((constructor))
static void _bug54_input_helper_init(void)
{
    static const uint32_t indices[] = {0x68};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x14656c78};
    static racebench_before_helper before_helpers[1] = {};
    _bug54_input_helper.interleave_num = 1;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0x14656c78;
}
static int _bug54_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[54], 1);
        _bug54_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug54_pattern(part);
}
static int _bug54_input_check(int part)
{
    return rb_input_check(&_bug54_input_helper, 54, part, _bug54_bug_callback);
}
void racebench_bug54(int part)
{
    rb_bug_synchronize(&_bug54_bug_helper, 54, part, _bug54_input_check);
}

static void _bug55_pattern_clean(void);
static int _bug55_pattern(int part);
static pthread_mutex_t _bug55_mutex1;
static pthread_cond_t _bug55_cond1;
static volatile __attribute__((used)) int _bug55_pred;
static volatile __attribute__((used)) int _bug55_sent;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    pthread_cond_init(&_bug55_cond1, NULL);
    _bug55_pred = 0;
    _bug55_sent = 0;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug55_mutex1);
        _bug55_pred = 1;
        pthread_cond_signal(&_bug55_cond1);
        _bug55_sent = 1;
        pthread_mutex_unlock(&_bug55_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug55_mutex1);
        if (_bug55_sent) {
            trigger = 1;
        }
        else {
            while (_bug55_pred == 0)
                pthread_cond_wait(&_bug55_cond1, &_bug55_mutex1);
        }
        _bug55_pred = 0;
        pthread_mutex_unlock(&_bug55_mutex1);
    }
    return trigger;
}

static void _bug55_pattern_clean(void);
static int _bug55_pattern(int part);
static racebench_bug_helper _bug55_bug_helper;
static racebench_input_helper _bug55_input_helper;
__attribute__((constructor))
static void _bug55_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ea};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xbb64ca0f, 0x93f63d2c};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x8ef51e1b};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0xde502556;
}
static int _bug55_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[55], 1);
        _bug55_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug55_pattern(part);
}
static int _bug55_input_check(int part)
{
    return rb_input_check(&_bug55_input_helper, 55, part, _bug55_bug_callback);
}
void racebench_bug55(int part)
{
    rb_bug_synchronize(&_bug55_bug_helper, 55, part, _bug55_input_check);
}

static void _bug56_pattern_clean(void);
static int _bug56_pattern(int part);
static pthread_mutex_t _bug56_mutex1;
static volatile __attribute__((used)) int _bug56_first_part;
static volatile __attribute__((used)) int _bug56_released;
static void _bug56_pattern_clean(void)
{
    pthread_mutex_init(&_bug56_mutex1, NULL);
    _bug56_first_part = -1;
    _bug56_released = -1;
}
static __attribute__((optimize(0))) int _bug56_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug56_mutex1) == 0)
                break;
            if (_bug56_released == 0) {
                _bug56_first_part = -1;
                _bug56_released = -1;
                return 1;
            }
        }
        if (_bug56_first_part == -1)
            _bug56_first_part = part;
        else {
            _bug56_first_part = -1;
            while (_bug56_released == -1) {}
            _bug56_released = -1;
        }
        pthread_mutex_unlock(&_bug56_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug56_mutex1);
        if (_bug56_first_part == -1) {
            _bug56_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug56_mutex1);
                _bug56_released = 1;
            }
            else {
                _bug56_released = 0;
                while (_bug56_released != -1) {}
                pthread_mutex_unlock(&_bug56_mutex1);
                return 1;
            }
        }
        else {
            _bug56_first_part = -1;
            pthread_mutex_unlock(&_bug56_mutex1);
        }
    }
    return 0;
}

static void _bug56_pattern_clean(void);
static int _bug56_pattern(int part);
static racebench_bug_helper _bug56_bug_helper;
static racebench_input_helper _bug56_input_helper;
__attribute__((constructor))
static void _bug56_input_helper_init(void)
{
    static const uint32_t indices[] = {0x88};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x94464bff};
    static racebench_before_helper before_helpers[0] = {};
    _bug56_input_helper.interleave_num = 0;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0x94464bff;
}
static int _bug56_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[56], 1);
        _bug56_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug56_pattern(part);
}
static int _bug56_input_check(int part)
{
    return rb_input_check(&_bug56_input_helper, 56, part, _bug56_bug_callback);
}
void racebench_bug56(int part)
{
    rb_bug_synchronize(&_bug56_bug_helper, 56, part, _bug56_input_check);
}

static void _bug57_pattern_clean(void);
static int _bug57_pattern(int part);
static pthread_mutex_t _bug57_mutex1;
static volatile __attribute__((used)) int _bug57_first_part;
static void _bug57_pattern_clean(void)
{
    pthread_mutex_init(&_bug57_mutex1, NULL);
    _bug57_first_part = -1;
}
static __attribute__((optimize(0))) int _bug57_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug57_mutex1);
        if (_bug57_first_part == -1)
            _bug57_first_part = part;
        else
            _bug57_first_part = -1;
        pthread_mutex_unlock(&_bug57_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug57_mutex1);
        if (_bug57_first_part == -1) {
            _bug57_first_part = part;
            trigger = 1;
        }
        else
            _bug57_first_part = -1;
        pthread_mutex_unlock(&_bug57_mutex1);
    }
    return trigger;
}

static void _bug57_pattern_clean(void);
static int _bug57_pattern(int part);
static racebench_bug_helper _bug57_bug_helper;
static racebench_input_helper _bug57_input_helper;
__attribute__((constructor))
static void _bug57_input_helper_init(void)
{
    static const uint32_t indices[] = {0x214};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xbd2a70, 0xb14f48b9};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa113c0ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug57_input_helper.interleave_num = 3;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0x532033d7;
}
static int _bug57_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[57], 1);
        _bug57_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug57_pattern(part);
}
static int _bug57_input_check(int part)
{
    return rb_input_check(&_bug57_input_helper, 57, part, _bug57_bug_callback);
}
void racebench_bug57(int part)
{
    rb_bug_synchronize(&_bug57_bug_helper, 57, part, _bug57_input_check);
}

static void _bug58_pattern_clean(void);
static int _bug58_pattern(int part);
static pthread_mutex_t _bug58_mutex1;
static volatile __attribute__((used)) int _bug58_first_part;
static void _bug58_pattern_clean(void)
{
    pthread_mutex_init(&_bug58_mutex1, NULL);
    _bug58_first_part = -1;
}
static __attribute__((optimize(0))) int _bug58_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug58_mutex1);
        if (_bug58_first_part == -1)
            _bug58_first_part = part;
        else
            _bug58_first_part = -1;
        pthread_mutex_unlock(&_bug58_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug58_mutex1);
        if (_bug58_first_part == -1) {
            _bug58_first_part = part;
            trigger = 1;
        }
        else
            _bug58_first_part = -1;
        pthread_mutex_unlock(&_bug58_mutex1);
    }
    return trigger;
}

static void _bug58_pattern_clean(void);
static int _bug58_pattern(int part);
static racebench_bug_helper _bug58_bug_helper;
static racebench_input_helper _bug58_input_helper;
__attribute__((constructor))
static void _bug58_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ef};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd2062000, 0x6c686646};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x3aa1cd9b};
    static racebench_before_helper before_helpers[3] = {};
    _bug58_input_helper.interleave_num = 3;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0x791053e1;
}
static int _bug58_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[58], 1);
        _bug58_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug58_pattern(part);
}
static int _bug58_input_check(int part)
{
    return rb_input_check(&_bug58_input_helper, 58, part, _bug58_bug_callback);
}
void racebench_bug58(int part)
{
    rb_bug_synchronize(&_bug58_bug_helper, 58, part, _bug58_input_check);
}

static void _bug59_pattern_clean(void);
static int _bug59_pattern(int part);
static pthread_mutex_t _bug59_mutex1;
static pthread_cond_t _bug59_cond1;
static volatile __attribute__((used)) int* volatile _bug59_ptr;
static volatile __attribute__((used)) int _bug59_val;
static volatile __attribute__((used)) int _bug59_pred;
static void _bug59_pattern_clean(void)
{
    pthread_mutex_init(&_bug59_mutex1, NULL);
    pthread_cond_init(&_bug59_cond1, NULL);
    _bug59_ptr = NULL;
    _bug59_val = 0;
    _bug59_pred = 0;
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug59_pred = 0;
        pthread_cond_signal(&_bug59_cond1);
        pthread_mutex_lock(&_bug59_mutex1);
        _bug59_ptr = &_bug59_val;
        _bug59_pred = 1;
        pthread_mutex_unlock(&_bug59_mutex1);
        pthread_cond_signal(&_bug59_cond1);
    }
    else {
        pthread_mutex_lock(&_bug59_mutex1);
        if (!_bug59_pred)
            pthread_cond_wait(&_bug59_cond1, &_bug59_mutex1);
        if (_bug59_ptr == NULL)
            trigger = 1;
        _bug59_ptr = NULL;
        _bug59_pred = 0;
        pthread_mutex_unlock(&_bug59_mutex1);
    }
    return trigger;
}

static void _bug59_pattern_clean(void);
static int _bug59_pattern(int part);
static racebench_bug_helper _bug59_bug_helper;
static racebench_input_helper _bug59_input_helper;
__attribute__((constructor))
static void _bug59_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ef};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x52134ee0};
    static racebench_before_helper before_helpers[0] = {};
    _bug59_input_helper.interleave_num = 0;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0x52134ee0;
}
static int _bug59_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[59], 1);
        _bug59_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug59_pattern(part);
}
static int _bug59_input_check(int part)
{
    return rb_input_check(&_bug59_input_helper, 59, part, _bug59_bug_callback);
}
void racebench_bug59(int part)
{
    rb_bug_synchronize(&_bug59_bug_helper, 59, part, _bug59_input_check);
}

static void _bug60_pattern_clean(void);
static int _bug60_pattern(int part);
static pthread_mutex_t _bug60_mutex1;
static pthread_cond_t _bug60_cond1;
static volatile __attribute__((used)) int* volatile _bug60_ptr;
static volatile __attribute__((used)) int _bug60_val;
static volatile __attribute__((used)) int _bug60_pred;
static void _bug60_pattern_clean(void)
{
    pthread_mutex_init(&_bug60_mutex1, NULL);
    pthread_cond_init(&_bug60_cond1, NULL);
    _bug60_ptr = NULL;
    _bug60_val = 0;
    _bug60_pred = 0;
}
static __attribute__((optimize(0))) int _bug60_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug60_pred = 0;
        pthread_cond_signal(&_bug60_cond1);
        pthread_mutex_lock(&_bug60_mutex1);
        _bug60_ptr = &_bug60_val;
        _bug60_pred = 1;
        pthread_mutex_unlock(&_bug60_mutex1);
        pthread_cond_signal(&_bug60_cond1);
    }
    else {
        pthread_mutex_lock(&_bug60_mutex1);
        if (!_bug60_pred)
            pthread_cond_wait(&_bug60_cond1, &_bug60_mutex1);
        if (_bug60_ptr == NULL)
            trigger = 1;
        _bug60_ptr = NULL;
        _bug60_pred = 0;
        pthread_mutex_unlock(&_bug60_mutex1);
    }
    return trigger;
}

static void _bug60_pattern_clean(void);
static int _bug60_pattern(int part);
static racebench_bug_helper _bug60_bug_helper;
static racebench_input_helper _bug60_input_helper;
__attribute__((constructor))
static void _bug60_input_helper_init(void)
{
    static const uint32_t indices[] = {0xe};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug60_input_helper.interleave_num = 1;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0xa1182f6e;
}
static int _bug60_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[60], 1);
        _bug60_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug60_pattern(part);
}
static int _bug60_input_check(int part)
{
    return rb_input_check(&_bug60_input_helper, 60, part, _bug60_bug_callback);
}
void racebench_bug60(int part)
{
    rb_bug_synchronize(&_bug60_bug_helper, 60, part, _bug60_input_check);
}

static void _bug61_pattern_clean(void);
static int _bug61_pattern(int part);
static pthread_mutex_t _bug61_mutex1;
static pthread_mutex_t _bug61_mutex2;
static void _bug61_pattern_clean(void)
{
    pthread_mutex_init(&_bug61_mutex1, NULL);
    pthread_mutex_init(&_bug61_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug61_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug61_mutex1);
        if (pthread_mutex_trylock(&_bug61_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug61_mutex2);
        pthread_mutex_unlock(&_bug61_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug61_mutex2);
        if (pthread_mutex_trylock(&_bug61_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug61_mutex1);
        pthread_mutex_unlock(&_bug61_mutex2);
    }
    return trigger;
}

static void _bug61_pattern_clean(void);
static int _bug61_pattern(int part);
static racebench_bug_helper _bug61_bug_helper;
static racebench_input_helper _bug61_input_helper;
__attribute__((constructor))
static void _bug61_input_helper_init(void)
{
    static const uint32_t indices[] = {0x24};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xcfe37747};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1187cae};
    static racebench_before_helper before_helpers[2] = {};
    _bug61_input_helper.interleave_num = 2;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0x70fbf3f5;
}
static int _bug61_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[61], 1);
        _bug61_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug61_pattern(part);
}
static int _bug61_input_check(int part)
{
    return rb_input_check(&_bug61_input_helper, 61, part, _bug61_bug_callback);
}
void racebench_bug61(int part)
{
    rb_bug_synchronize(&_bug61_bug_helper, 61, part, _bug61_input_check);
}

static void _bug62_pattern_clean(void);
static int _bug62_pattern(int part);
static pthread_mutex_t _bug62_mutex1;
static pthread_cond_t _bug62_cond1;
static volatile __attribute__((used)) int _bug62_pred;
static volatile __attribute__((used)) int _bug62_sent;
static void _bug62_pattern_clean(void)
{
    pthread_mutex_init(&_bug62_mutex1, NULL);
    pthread_cond_init(&_bug62_cond1, NULL);
    _bug62_pred = 0;
    _bug62_sent = 0;
}
static __attribute__((optimize(0))) int _bug62_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug62_mutex1);
        _bug62_pred = 1;
        pthread_cond_signal(&_bug62_cond1);
        _bug62_sent = 1;
        pthread_mutex_unlock(&_bug62_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug62_mutex1);
        if (_bug62_sent) {
            trigger = 1;
        }
        else {
            while (_bug62_pred == 0)
                pthread_cond_wait(&_bug62_cond1, &_bug62_mutex1);
        }
        _bug62_pred = 0;
        pthread_mutex_unlock(&_bug62_mutex1);
    }
    return trigger;
}

static void _bug62_pattern_clean(void);
static int _bug62_pattern(int part);
static racebench_bug_helper _bug62_bug_helper;
static racebench_input_helper _bug62_input_helper;
__attribute__((constructor))
static void _bug62_input_helper_init(void)
{
    static const uint32_t indices[] = {0xd9};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x1f63f1e4, 0x84aa6061};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11530de};
    static racebench_before_helper before_helpers[3] = {};
    _bug62_input_helper.interleave_num = 3;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0x45238323;
}
static int _bug62_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[62], 1);
        _bug62_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug62_pattern(part);
}
static int _bug62_input_check(int part)
{
    return rb_input_check(&_bug62_input_helper, 62, part, _bug62_bug_callback);
}
void racebench_bug62(int part)
{
    rb_bug_synchronize(&_bug62_bug_helper, 62, part, _bug62_input_check);
}

static void _bug63_pattern_clean(void);
static int _bug63_pattern(int part);
static pthread_mutex_t _bug63_mutex1;
static volatile __attribute__((used)) int _bug63_first_part;
static void _bug63_pattern_clean(void)
{
    pthread_mutex_init(&_bug63_mutex1, NULL);
    _bug63_first_part = -1;
}
static __attribute__((optimize(0))) int _bug63_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug63_mutex1);
        if (_bug63_first_part == -1)
            _bug63_first_part = part;
        else
            _bug63_first_part = -1;
        pthread_mutex_unlock(&_bug63_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug63_mutex1);
        if (_bug63_first_part == -1) {
            _bug63_first_part = part;
            trigger = 1;
        }
        else
            _bug63_first_part = -1;
        pthread_mutex_unlock(&_bug63_mutex1);
    }
    return trigger;
}

static void _bug63_pattern_clean(void);
static int _bug63_pattern(int part);
static racebench_bug_helper _bug63_bug_helper;
static racebench_input_helper _bug63_input_helper;
__attribute__((constructor))
static void _bug63_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf12681be};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x7426dfbf};
    static racebench_before_helper before_helpers[2] = {};
    _bug63_input_helper.interleave_num = 2;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x654d617d;
}
static int _bug63_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[63], 1);
        _bug63_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug63_pattern(part);
}
static int _bug63_input_check(int part)
{
    return rb_input_check(&_bug63_input_helper, 63, part, _bug63_bug_callback);
}
void racebench_bug63(int part)
{
    rb_bug_synchronize(&_bug63_bug_helper, 63, part, _bug63_input_check);
}

static void _bug64_pattern_clean(void);
static int _bug64_pattern(int part);
static pthread_mutex_t _bug64_mutex1;
static pthread_cond_t _bug64_cond1;
static volatile __attribute__((used)) int _bug64_pred;
static volatile __attribute__((used)) int _bug64_sent;
static void _bug64_pattern_clean(void)
{
    pthread_mutex_init(&_bug64_mutex1, NULL);
    pthread_cond_init(&_bug64_cond1, NULL);
    _bug64_pred = 0;
    _bug64_sent = 0;
}
static __attribute__((optimize(0))) int _bug64_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug64_mutex1);
        _bug64_pred = 1;
        pthread_cond_signal(&_bug64_cond1);
        _bug64_sent = 1;
        pthread_mutex_unlock(&_bug64_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug64_mutex1);
        if (_bug64_sent) {
            trigger = 1;
        }
        else {
            while (_bug64_pred == 0)
                pthread_cond_wait(&_bug64_cond1, &_bug64_mutex1);
        }
        _bug64_pred = 0;
        pthread_mutex_unlock(&_bug64_mutex1);
    }
    return trigger;
}

static void _bug64_pattern_clean(void);
static int _bug64_pattern(int part);
static racebench_bug_helper _bug64_bug_helper;
static racebench_input_helper _bug64_input_helper;
__attribute__((constructor))
static void _bug64_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1f0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182fbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug64_input_helper.interleave_num = 0;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0xa1182fbe;
}
static int _bug64_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[64], 1);
        _bug64_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug64_pattern(part);
}
static int _bug64_input_check(int part)
{
    return rb_input_check(&_bug64_input_helper, 64, part, _bug64_bug_callback);
}
void racebench_bug64(int part)
{
    rb_bug_synchronize(&_bug64_bug_helper, 64, part, _bug64_input_check);
}

static void _bug65_pattern_clean(void);
static int _bug65_pattern(int part);
static pthread_mutex_t _bug65_mutex1;
static volatile __attribute__((used)) int* volatile _bug65_ptr;
static volatile __attribute__((used)) int _bug65_val;
static void _bug65_pattern_clean(void)
{
    pthread_mutex_init(&_bug65_mutex1, NULL);
    _bug65_ptr = NULL;
    _bug65_val = 0;
}
static __attribute__((optimize(0))) int _bug65_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug65_mutex1);
        _bug65_ptr = &_bug65_val;
        pthread_mutex_unlock(&_bug65_mutex1);
        pthread_mutex_lock(&_bug65_mutex1);
        if (_bug65_ptr == NULL)
            trigger = 1;
        _bug65_ptr = NULL;
        pthread_mutex_unlock(&_bug65_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug65_mutex1);
        _bug65_ptr = &_bug65_val;
        if (_bug65_ptr == NULL)
            trigger = 1;
        _bug65_ptr = NULL;
        pthread_mutex_unlock(&_bug65_mutex1);
    }
    return trigger;
}

static void _bug65_pattern_clean(void);
static int _bug65_pattern(int part);
static racebench_bug_helper _bug65_bug_helper;
static racebench_input_helper _bug65_input_helper;
__attribute__((constructor))
static void _bug65_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x857ad179};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x8e12a96f};
    static racebench_before_helper before_helpers[2] = {};
    _bug65_input_helper.interleave_num = 2;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0x138d7ae8;
}
static int _bug65_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[65], 1);
        _bug65_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug65_pattern(part);
}
static int _bug65_input_check(int part)
{
    return rb_input_check(&_bug65_input_helper, 65, part, _bug65_bug_callback);
}
void racebench_bug65(int part)
{
    rb_bug_synchronize(&_bug65_bug_helper, 65, part, _bug65_input_check);
}

static void _bug66_pattern_clean(void);
static int _bug66_pattern(int part);
static pthread_mutex_t _bug66_mutex1;
static volatile __attribute__((used)) int* volatile _bug66_ptr;
static volatile __attribute__((used)) int _bug66_val;
static void _bug66_pattern_clean(void)
{
    pthread_mutex_init(&_bug66_mutex1, NULL);
    _bug66_ptr = NULL;
    _bug66_val = 0;
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug66_mutex1);
        _bug66_ptr = &_bug66_val;
        pthread_mutex_unlock(&_bug66_mutex1);
        pthread_mutex_lock(&_bug66_mutex1);
        if (_bug66_ptr == NULL)
            trigger = 1;
        _bug66_ptr = NULL;
        pthread_mutex_unlock(&_bug66_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug66_mutex1);
        _bug66_ptr = &_bug66_val;
        if (_bug66_ptr == NULL)
            trigger = 1;
        _bug66_ptr = NULL;
        pthread_mutex_unlock(&_bug66_mutex1);
    }
    return trigger;
}

static void _bug66_pattern_clean(void);
static int _bug66_pattern(int part);
static racebench_bug_helper _bug66_bug_helper;
static racebench_input_helper _bug66_input_helper;
__attribute__((constructor))
static void _bug66_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xd5732fca};
    static racebench_before_helper before_helpers[1] = {};
    _bug66_input_helper.interleave_num = 1;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0xd5732fca;
}
static int _bug66_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[66], 1);
        _bug66_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug66_pattern(part);
}
static int _bug66_input_check(int part)
{
    return rb_input_check(&_bug66_input_helper, 66, part, _bug66_bug_callback);
}
void racebench_bug66(int part)
{
    rb_bug_synchronize(&_bug66_bug_helper, 66, part, _bug66_input_check);
}

static void _bug67_pattern_clean(void);
static int _bug67_pattern(int part);
static pthread_mutex_t _bug67_mutex1;
static pthread_cond_t _bug67_cond1;
static volatile __attribute__((used)) int _bug67_pred;
static volatile __attribute__((used)) int _bug67_sent;
static void _bug67_pattern_clean(void)
{
    pthread_mutex_init(&_bug67_mutex1, NULL);
    pthread_cond_init(&_bug67_cond1, NULL);
    _bug67_pred = 0;
    _bug67_sent = 0;
}
static __attribute__((optimize(0))) int _bug67_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug67_mutex1);
        _bug67_pred = 1;
        pthread_cond_signal(&_bug67_cond1);
        _bug67_sent = 1;
        pthread_mutex_unlock(&_bug67_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug67_mutex1);
        if (_bug67_sent) {
            trigger = 1;
        }
        else {
            while (_bug67_pred == 0)
                pthread_cond_wait(&_bug67_cond1, &_bug67_mutex1);
        }
        _bug67_pred = 0;
        pthread_mutex_unlock(&_bug67_mutex1);
    }
    return trigger;
}

static void _bug67_pattern_clean(void);
static int _bug67_pattern(int part);
static racebench_bug_helper _bug67_bug_helper;
static racebench_input_helper _bug67_input_helper;
__attribute__((constructor))
static void _bug67_input_helper_init(void)
{
    static const uint32_t indices[] = {0x23};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xadc4693f};
    static racebench_before_helper before_helpers[0] = {};
    _bug67_input_helper.interleave_num = 0;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0xadc4693f;
}
static int _bug67_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[67], 1);
        _bug67_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug67_pattern(part);
}
static int _bug67_input_check(int part)
{
    return rb_input_check(&_bug67_input_helper, 67, part, _bug67_bug_callback);
}
void racebench_bug67(int part)
{
    rb_bug_synchronize(&_bug67_bug_helper, 67, part, _bug67_input_check);
}

static void _bug68_pattern_clean(void);
static int _bug68_pattern(int part);
static pthread_mutex_t _bug68_mutex1;
static pthread_cond_t _bug68_cond1;
static volatile __attribute__((used)) int _bug68_pred;
static volatile __attribute__((used)) int _bug68_sent;
static void _bug68_pattern_clean(void)
{
    pthread_mutex_init(&_bug68_mutex1, NULL);
    pthread_cond_init(&_bug68_cond1, NULL);
    _bug68_pred = 0;
    _bug68_sent = 0;
}
static __attribute__((optimize(0))) int _bug68_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug68_mutex1);
        _bug68_pred = 1;
        pthread_cond_signal(&_bug68_cond1);
        _bug68_sent = 1;
        pthread_mutex_unlock(&_bug68_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug68_mutex1);
        if (_bug68_sent) {
            trigger = 1;
        }
        else {
            while (_bug68_pred == 0)
                pthread_cond_wait(&_bug68_cond1, &_bug68_mutex1);
        }
        _bug68_pred = 0;
        pthread_mutex_unlock(&_bug68_mutex1);
    }
    return trigger;
}

static void _bug68_pattern_clean(void);
static int _bug68_pattern(int part);
static racebench_bug_helper _bug68_bug_helper;
static racebench_input_helper _bug68_input_helper;
__attribute__((constructor))
static void _bug68_input_helper_init(void)
{
    static const uint32_t indices[] = {0x20e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4b72b30e, 0x7ff02c15};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1152a6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug68_input_helper.interleave_num = 3;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0x6c780991;
}
static int _bug68_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[68], 1);
        _bug68_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug68_pattern(part);
}
static int _bug68_input_check(int part)
{
    return rb_input_check(&_bug68_input_helper, 68, part, _bug68_bug_callback);
}
void racebench_bug68(int part)
{
    rb_bug_synchronize(&_bug68_bug_helper, 68, part, _bug68_input_check);
}

static void _bug69_pattern_clean(void);
static int _bug69_pattern(int part);
static pthread_mutex_t _bug69_mutex1;
static pthread_cond_t _bug69_cond1;
static volatile __attribute__((used)) int _bug69_pred;
static volatile __attribute__((used)) int _bug69_sent;
static void _bug69_pattern_clean(void)
{
    pthread_mutex_init(&_bug69_mutex1, NULL);
    pthread_cond_init(&_bug69_cond1, NULL);
    _bug69_pred = 0;
    _bug69_sent = 0;
}
static __attribute__((optimize(0))) int _bug69_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug69_mutex1);
        _bug69_pred = 1;
        pthread_cond_signal(&_bug69_cond1);
        _bug69_sent = 1;
        pthread_mutex_unlock(&_bug69_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug69_mutex1);
        if (_bug69_sent) {
            trigger = 1;
        }
        else {
            while (_bug69_pred == 0)
                pthread_cond_wait(&_bug69_cond1, &_bug69_mutex1);
        }
        _bug69_pred = 0;
        pthread_mutex_unlock(&_bug69_mutex1);
    }
    return trigger;
}

static void _bug69_pattern_clean(void);
static int _bug69_pattern(int part);
static racebench_bug_helper _bug69_bug_helper;
static racebench_input_helper _bug69_input_helper;
__attribute__((constructor))
static void _bug69_input_helper_init(void)
{
    static const uint32_t indices[] = {0x3e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa116a4ee};
    static racebench_before_helper before_helpers[1] = {};
    _bug69_input_helper.interleave_num = 1;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0xa116a4ee;
}
static int _bug69_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[69], 1);
        _bug69_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug69_pattern(part);
}
static int _bug69_input_check(int part)
{
    return rb_input_check(&_bug69_input_helper, 69, part, _bug69_bug_callback);
}
void racebench_bug69(int part)
{
    rb_bug_synchronize(&_bug69_bug_helper, 69, part, _bug69_input_check);
}

static void _bug70_pattern_clean(void);
static int _bug70_pattern(int part);
static pthread_mutex_t _bug70_mutex1;
static pthread_cond_t _bug70_cond1;
static volatile __attribute__((used)) int* volatile _bug70_ptr;
static volatile __attribute__((used)) int _bug70_val;
static volatile __attribute__((used)) int _bug70_pred;
static void _bug70_pattern_clean(void)
{
    pthread_mutex_init(&_bug70_mutex1, NULL);
    pthread_cond_init(&_bug70_cond1, NULL);
    _bug70_ptr = NULL;
    _bug70_val = 0;
    _bug70_pred = 0;
}
static __attribute__((optimize(0))) int _bug70_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug70_pred = 0;
        pthread_cond_signal(&_bug70_cond1);
        pthread_mutex_lock(&_bug70_mutex1);
        _bug70_ptr = &_bug70_val;
        _bug70_pred = 1;
        pthread_mutex_unlock(&_bug70_mutex1);
        pthread_cond_signal(&_bug70_cond1);
    }
    else {
        pthread_mutex_lock(&_bug70_mutex1);
        if (!_bug70_pred)
            pthread_cond_wait(&_bug70_cond1, &_bug70_mutex1);
        if (_bug70_ptr == NULL)
            trigger = 1;
        _bug70_ptr = NULL;
        _bug70_pred = 0;
        pthread_mutex_unlock(&_bug70_mutex1);
    }
    return trigger;
}

static void _bug70_pattern_clean(void);
static int _bug70_pattern(int part);
static racebench_bug_helper _bug70_bug_helper;
static racebench_input_helper _bug70_input_helper;
__attribute__((constructor))
static void _bug70_input_helper_init(void)
{
    static const uint32_t indices[] = {0xe};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x6fd73151, 0x6456b825};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x6421fc40};
    static racebench_before_helper before_helpers[3] = {};
    _bug70_input_helper.interleave_num = 3;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0x384fe5b6;
}
static int _bug70_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[70], 1);
        _bug70_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug70_pattern(part);
}
static int _bug70_input_check(int part)
{
    return rb_input_check(&_bug70_input_helper, 70, part, _bug70_bug_callback);
}
void racebench_bug70(int part)
{
    rb_bug_synchronize(&_bug70_bug_helper, 70, part, _bug70_input_check);
}

static void _bug71_pattern_clean(void);
static int _bug71_pattern(int part);
static pthread_mutex_t _bug71_mutex1;
static volatile __attribute__((used)) int* volatile _bug71_ptr;
static volatile __attribute__((used)) int _bug71_val;
static void _bug71_pattern_clean(void)
{
    pthread_mutex_init(&_bug71_mutex1, NULL);
    _bug71_ptr = NULL;
    _bug71_val = 0;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug71_mutex1);
        _bug71_ptr = &_bug71_val;
        pthread_mutex_unlock(&_bug71_mutex1);
        pthread_mutex_lock(&_bug71_mutex1);
        if (_bug71_ptr == NULL)
            trigger = 1;
        _bug71_ptr = NULL;
        pthread_mutex_unlock(&_bug71_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug71_mutex1);
        _bug71_ptr = &_bug71_val;
        if (_bug71_ptr == NULL)
            trigger = 1;
        _bug71_ptr = NULL;
        pthread_mutex_unlock(&_bug71_mutex1);
    }
    return trigger;
}

static void _bug71_pattern_clean(void);
static int _bug71_pattern(int part);
static racebench_bug_helper _bug71_bug_helper;
static racebench_input_helper _bug71_input_helper;
__attribute__((constructor))
static void _bug71_input_helper_init(void)
{
    static const uint32_t indices[] = {0x20c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x23555775, 0x9dac8191};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115ffae};
    static racebench_before_helper before_helpers[3] = {};
    _bug71_input_helper.interleave_num = 3;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0x6217d8b4;
}
static int _bug71_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[71], 1);
        _bug71_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug71_pattern(part);
}
static int _bug71_input_check(int part)
{
    return rb_input_check(&_bug71_input_helper, 71, part, _bug71_bug_callback);
}
void racebench_bug71(int part)
{
    rb_bug_synchronize(&_bug71_bug_helper, 71, part, _bug71_input_check);
}

static void _bug72_pattern_clean(void);
static int _bug72_pattern(int part);
static pthread_mutex_t _bug72_mutex1;
static volatile __attribute__((used)) int _bug72_first_part;
static volatile __attribute__((used)) int _bug72_released;
static void _bug72_pattern_clean(void)
{
    pthread_mutex_init(&_bug72_mutex1, NULL);
    _bug72_first_part = -1;
    _bug72_released = -1;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug72_mutex1) == 0)
                break;
            if (_bug72_released == 0) {
                _bug72_first_part = -1;
                _bug72_released = -1;
                return 1;
            }
        }
        if (_bug72_first_part == -1)
            _bug72_first_part = part;
        else {
            _bug72_first_part = -1;
            while (_bug72_released == -1) {}
            _bug72_released = -1;
        }
        pthread_mutex_unlock(&_bug72_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug72_mutex1);
        if (_bug72_first_part == -1) {
            _bug72_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug72_mutex1);
                _bug72_released = 1;
            }
            else {
                _bug72_released = 0;
                while (_bug72_released != -1) {}
                pthread_mutex_unlock(&_bug72_mutex1);
                return 1;
            }
        }
        else {
            _bug72_first_part = -1;
            pthread_mutex_unlock(&_bug72_mutex1);
        }
    }
    return 0;
}

static void _bug72_pattern_clean(void);
static int _bug72_pattern(int part);
static racebench_bug_helper _bug72_bug_helper;
static racebench_input_helper _bug72_input_helper;
__attribute__((constructor))
static void _bug72_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1df};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdac66fdf, 0x898b9ea8};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1153c0e};
    static racebench_before_helper before_helpers[3] = {};
    _bug72_input_helper.interleave_num = 3;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0x5674a95;
}
static int _bug72_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[72], 1);
        _bug72_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug72_pattern(part);
}
static int _bug72_input_check(int part)
{
    return rb_input_check(&_bug72_input_helper, 72, part, _bug72_bug_callback);
}
void racebench_bug72(int part)
{
    rb_bug_synchronize(&_bug72_bug_helper, 72, part, _bug72_input_check);
}

static void _bug73_pattern_clean(void);
static int _bug73_pattern(int part);
static pthread_mutex_t _bug73_mutex1;
static volatile __attribute__((used)) int* volatile _bug73_ptr;
static volatile __attribute__((used)) int _bug73_val;
static void _bug73_pattern_clean(void)
{
    pthread_mutex_init(&_bug73_mutex1, NULL);
    _bug73_ptr = NULL;
    _bug73_val = 0;
}
static __attribute__((optimize(0))) int _bug73_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug73_mutex1);
        _bug73_ptr = &_bug73_val;
        pthread_mutex_unlock(&_bug73_mutex1);
        pthread_mutex_lock(&_bug73_mutex1);
        if (_bug73_ptr == NULL)
            trigger = 1;
        _bug73_ptr = NULL;
        pthread_mutex_unlock(&_bug73_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug73_mutex1);
        _bug73_ptr = &_bug73_val;
        if (_bug73_ptr == NULL)
            trigger = 1;
        _bug73_ptr = NULL;
        pthread_mutex_unlock(&_bug73_mutex1);
    }
    return trigger;
}

static void _bug73_pattern_clean(void);
static int _bug73_pattern(int part);
static racebench_bug_helper _bug73_bug_helper;
static racebench_input_helper _bug73_input_helper;
__attribute__((constructor))
static void _bug73_input_helper_init(void)
{
    static const uint32_t indices[] = {0xca};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1148c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug73_input_helper.interleave_num = 0;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0xa1148c4e;
}
static int _bug73_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[73], 1);
        _bug73_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug73_pattern(part);
}
static int _bug73_input_check(int part)
{
    return rb_input_check(&_bug73_input_helper, 73, part, _bug73_bug_callback);
}
void racebench_bug73(int part)
{
    rb_bug_synchronize(&_bug73_bug_helper, 73, part, _bug73_input_check);
}

static void _bug74_pattern_clean(void);
static int _bug74_pattern(int part);
static volatile __attribute__((used)) int _bug74_arr[3];
static volatile __attribute__((used)) int _bug74_val;
static volatile __attribute__((used)) int _bug74_pred;
static void _bug74_pattern_clean(void)
{
    _bug74_arr[0] = 0, _bug74_arr[1] = 2, _bug74_arr[2] = 1;
    _bug74_val = 0;
    _bug74_pred = 0;
}
static __attribute__((optimize(0))) int _bug74_pattern(int part)
{
    int trigger = 0;
    _bug74_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug74_val += _bug74_arr[i];
        }
        __sync_synchronize();
        while (!_bug74_pred)
            pthread_yield();
        trigger = _bug74_val != 3;
        _bug74_pred = 0;
        _bug74_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug74_val += _bug74_arr[i];
        }
        __sync_synchronize();
        _bug74_pred = 1;
    }
    return trigger;
}

static void _bug74_pattern_clean(void);
static int _bug74_pattern(int part);
static racebench_bug_helper _bug74_bug_helper;
static racebench_input_helper _bug74_input_helper;
__attribute__((constructor))
static void _bug74_input_helper_init(void)
{
    static const uint32_t indices[] = {0x82};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x54ab780f, 0xff312b08};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x3695b109};
    static racebench_before_helper before_helpers[3] = {};
    _bug74_input_helper.interleave_num = 3;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0x8a725420;
}
static int _bug74_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[74], 1);
        _bug74_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug74_pattern(part);
}
static int _bug74_input_check(int part)
{
    return rb_input_check(&_bug74_input_helper, 74, part, _bug74_bug_callback);
}
void racebench_bug74(int part)
{
    rb_bug_synchronize(&_bug74_bug_helper, 74, part, _bug74_input_check);
}

static void _bug75_pattern_clean(void);
static int _bug75_pattern(int part);
static pthread_mutex_t _bug75_mutex1;
static pthread_mutex_t _bug75_mutex2;
static void _bug75_pattern_clean(void)
{
    pthread_mutex_init(&_bug75_mutex1, NULL);
    pthread_mutex_init(&_bug75_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug75_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug75_mutex1);
        if (pthread_mutex_trylock(&_bug75_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug75_mutex2);
        pthread_mutex_unlock(&_bug75_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug75_mutex2);
        if (pthread_mutex_trylock(&_bug75_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug75_mutex1);
        pthread_mutex_unlock(&_bug75_mutex2);
    }
    return trigger;
}

static void _bug75_pattern_clean(void);
static int _bug75_pattern(int part);
static racebench_bug_helper _bug75_bug_helper;
static racebench_input_helper _bug75_input_helper;
__attribute__((constructor))
static void _bug75_input_helper_init(void)
{
    static const uint32_t indices[] = {0x22c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa117009e};
    static racebench_before_helper before_helpers[1] = {};
    _bug75_input_helper.interleave_num = 1;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xa117009e;
}
static int _bug75_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[75], 1);
        _bug75_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug75_pattern(part);
}
static int _bug75_input_check(int part)
{
    return rb_input_check(&_bug75_input_helper, 75, part, _bug75_bug_callback);
}
void racebench_bug75(int part)
{
    rb_bug_synchronize(&_bug75_bug_helper, 75, part, _bug75_input_check);
}

static void _bug76_pattern_clean(void);
static int _bug76_pattern(int part);
static pthread_mutex_t _bug76_mutex1;
static volatile __attribute__((used)) int _bug76_first_part;
static volatile __attribute__((used)) int _bug76_released;
static void _bug76_pattern_clean(void)
{
    pthread_mutex_init(&_bug76_mutex1, NULL);
    _bug76_first_part = -1;
    _bug76_released = -1;
}
static __attribute__((optimize(0))) int _bug76_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug76_mutex1) == 0)
                break;
            if (_bug76_released == 0) {
                _bug76_first_part = -1;
                _bug76_released = -1;
                return 1;
            }
        }
        if (_bug76_first_part == -1)
            _bug76_first_part = part;
        else {
            _bug76_first_part = -1;
            while (_bug76_released == -1) {}
            _bug76_released = -1;
        }
        pthread_mutex_unlock(&_bug76_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug76_mutex1);
        if (_bug76_first_part == -1) {
            _bug76_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug76_mutex1);
                _bug76_released = 1;
            }
            else {
                _bug76_released = 0;
                while (_bug76_released != -1) {}
                pthread_mutex_unlock(&_bug76_mutex1);
                return 1;
            }
        }
        else {
            _bug76_first_part = -1;
            pthread_mutex_unlock(&_bug76_mutex1);
        }
    }
    return 0;
}

static void _bug76_pattern_clean(void);
static int _bug76_pattern(int part);
static racebench_bug_helper _bug76_bug_helper;
static racebench_input_helper _bug76_input_helper;
__attribute__((constructor))
static void _bug76_input_helper_init(void)
{
    static const uint32_t indices[] = {0x229};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x93826382};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1136d6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0x3495d0f0;
}
static int _bug76_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[76], 1);
        _bug76_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug76_pattern(part);
}
static int _bug76_input_check(int part)
{
    return rb_input_check(&_bug76_input_helper, 76, part, _bug76_bug_callback);
}
void racebench_bug76(int part)
{
    rb_bug_synchronize(&_bug76_bug_helper, 76, part, _bug76_input_check);
}

static void _bug77_pattern_clean(void);
static int _bug77_pattern(int part);
static volatile __attribute__((used)) int _bug77_arr[3];
static volatile __attribute__((used)) int _bug77_val;
static volatile __attribute__((used)) int _bug77_pred;
static void _bug77_pattern_clean(void)
{
    _bug77_arr[0] = 0, _bug77_arr[1] = 2, _bug77_arr[2] = 1;
    _bug77_val = 0;
    _bug77_pred = 0;
}
static __attribute__((optimize(0))) int _bug77_pattern(int part)
{
    int trigger = 0;
    _bug77_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug77_val += _bug77_arr[i];
        }
        __sync_synchronize();
        while (!_bug77_pred)
            pthread_yield();
        trigger = _bug77_val != 3;
        _bug77_pred = 0;
        _bug77_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug77_val += _bug77_arr[i];
        }
        __sync_synchronize();
        _bug77_pred = 1;
    }
    return trigger;
}

static void _bug77_pattern_clean(void);
static int _bug77_pattern(int part);
static racebench_bug_helper _bug77_bug_helper;
static racebench_input_helper _bug77_input_helper;
__attribute__((constructor))
static void _bug77_input_helper_init(void)
{
    static const uint32_t indices[] = {0x94};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xaaa6f5ae, 0xfc7971ab};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x74856c9e};
    static racebench_before_helper before_helpers[3] = {};
    _bug77_input_helper.interleave_num = 3;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0x1ba5d3f7;
}
static int _bug77_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[77], 1);
        _bug77_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug77_pattern(part);
}
static int _bug77_input_check(int part)
{
    return rb_input_check(&_bug77_input_helper, 77, part, _bug77_bug_callback);
}
void racebench_bug77(int part)
{
    rb_bug_synchronize(&_bug77_bug_helper, 77, part, _bug77_input_check);
}

static void _bug78_pattern_clean(void);
static int _bug78_pattern(int part);
static pthread_mutex_t _bug78_mutex1;
static pthread_cond_t _bug78_cond1;
static volatile __attribute__((used)) int* volatile _bug78_ptr;
static volatile __attribute__((used)) int _bug78_val;
static volatile __attribute__((used)) int _bug78_pred;
static void _bug78_pattern_clean(void)
{
    pthread_mutex_init(&_bug78_mutex1, NULL);
    pthread_cond_init(&_bug78_cond1, NULL);
    _bug78_ptr = NULL;
    _bug78_val = 0;
    _bug78_pred = 0;
}
static __attribute__((optimize(0))) int _bug78_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug78_pred = 0;
        pthread_cond_signal(&_bug78_cond1);
        pthread_mutex_lock(&_bug78_mutex1);
        _bug78_ptr = &_bug78_val;
        _bug78_pred = 1;
        pthread_mutex_unlock(&_bug78_mutex1);
        pthread_cond_signal(&_bug78_cond1);
    }
    else {
        pthread_mutex_lock(&_bug78_mutex1);
        if (!_bug78_pred)
            pthread_cond_wait(&_bug78_cond1, &_bug78_mutex1);
        if (_bug78_ptr == NULL)
            trigger = 1;
        _bug78_ptr = NULL;
        _bug78_pred = 0;
        pthread_mutex_unlock(&_bug78_mutex1);
    }
    return trigger;
}

static void _bug78_pattern_clean(void);
static int _bug78_pattern(int part);
static racebench_bug_helper _bug78_bug_helper;
static racebench_input_helper _bug78_input_helper;
__attribute__((constructor))
static void _bug78_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug78_input_helper.interleave_num = 0;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0xa1182cbe;
}
static int _bug78_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[78], 1);
        _bug78_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug78_pattern(part);
}
static int _bug78_input_check(int part)
{
    return rb_input_check(&_bug78_input_helper, 78, part, _bug78_bug_callback);
}
void racebench_bug78(int part)
{
    rb_bug_synchronize(&_bug78_bug_helper, 78, part, _bug78_input_check);
}

static void _bug79_pattern_clean(void);
static int _bug79_pattern(int part);
static pthread_mutex_t _bug79_mutex1;
static pthread_cond_t _bug79_cond1;
static volatile __attribute__((used)) int* volatile _bug79_ptr;
static volatile __attribute__((used)) int _bug79_val;
static volatile __attribute__((used)) int _bug79_pred;
static void _bug79_pattern_clean(void)
{
    pthread_mutex_init(&_bug79_mutex1, NULL);
    pthread_cond_init(&_bug79_cond1, NULL);
    _bug79_ptr = NULL;
    _bug79_val = 0;
    _bug79_pred = 0;
}
static __attribute__((optimize(0))) int _bug79_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug79_pred = 0;
        pthread_cond_signal(&_bug79_cond1);
        pthread_mutex_lock(&_bug79_mutex1);
        _bug79_ptr = &_bug79_val;
        _bug79_pred = 1;
        pthread_mutex_unlock(&_bug79_mutex1);
        pthread_cond_signal(&_bug79_cond1);
    }
    else {
        pthread_mutex_lock(&_bug79_mutex1);
        if (!_bug79_pred)
            pthread_cond_wait(&_bug79_cond1, &_bug79_mutex1);
        if (_bug79_ptr == NULL)
            trigger = 1;
        _bug79_ptr = NULL;
        _bug79_pred = 0;
        pthread_mutex_unlock(&_bug79_mutex1);
    }
    return trigger;
}

static void _bug79_pattern_clean(void);
static int _bug79_pattern(int part);
static racebench_bug_helper _bug79_bug_helper;
static racebench_input_helper _bug79_input_helper;
__attribute__((constructor))
static void _bug79_input_helper_init(void)
{
    static const uint32_t indices[] = {0xaf};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x721879e5};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[2] = {};
    _bug79_input_helper.interleave_num = 2;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0x1330a6b3;
}
static int _bug79_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[79], 1);
        _bug79_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug79_pattern(part);
}
static int _bug79_input_check(int part)
{
    return rb_input_check(&_bug79_input_helper, 79, part, _bug79_bug_callback);
}
void racebench_bug79(int part)
{
    rb_bug_synchronize(&_bug79_bug_helper, 79, part, _bug79_input_check);
}

static void _bug80_pattern_clean(void);
static int _bug80_pattern(int part);
static pthread_mutex_t _bug80_mutex1;
static volatile __attribute__((used)) int _bug80_first_part;
static void _bug80_pattern_clean(void)
{
    pthread_mutex_init(&_bug80_mutex1, NULL);
    _bug80_first_part = -1;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug80_mutex1);
        if (_bug80_first_part == -1)
            _bug80_first_part = part;
        else
            _bug80_first_part = -1;
        pthread_mutex_unlock(&_bug80_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug80_mutex1);
        if (_bug80_first_part == -1) {
            _bug80_first_part = part;
            trigger = 1;
        }
        else
            _bug80_first_part = -1;
        pthread_mutex_unlock(&_bug80_mutex1);
    }
    return trigger;
}

static void _bug80_pattern_clean(void);
static int _bug80_pattern(int part);
static racebench_bug_helper _bug80_bug_helper;
static racebench_input_helper _bug80_input_helper;
__attribute__((constructor))
static void _bug80_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x834b2542};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11526de};
    static racebench_before_helper before_helpers[2] = {};
    _bug80_input_helper.interleave_num = 2;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0x24604c20;
}
static int _bug80_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[80], 1);
        _bug80_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug80_pattern(part);
}
static int _bug80_input_check(int part)
{
    return rb_input_check(&_bug80_input_helper, 80, part, _bug80_bug_callback);
}
void racebench_bug80(int part)
{
    rb_bug_synchronize(&_bug80_bug_helper, 80, part, _bug80_input_check);
}

static void _bug81_pattern_clean(void);
static int _bug81_pattern(int part);
static pthread_mutex_t _bug81_mutex1;
static volatile __attribute__((used)) int _bug81_val;
static void _bug81_pattern_clean(void)
{
    pthread_mutex_init(&_bug81_mutex1, NULL);
    _bug81_val = 0;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug81_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug81_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug81_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug81_pattern_clean(void);
static int _bug81_pattern(int part);
static racebench_bug_helper _bug81_bug_helper;
static racebench_input_helper _bug81_input_helper;
__attribute__((constructor))
static void _bug81_input_helper_init(void)
{
    static const uint32_t indices[] = {0x46};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug81_input_helper.interleave_num = 1;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xa1182c6e;
}
static int _bug81_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[81], 1);
        _bug81_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug81_pattern(part);
}
static int _bug81_input_check(int part)
{
    return rb_input_check(&_bug81_input_helper, 81, part, _bug81_bug_callback);
}
void racebench_bug81(int part)
{
    rb_bug_synchronize(&_bug81_bug_helper, 81, part, _bug81_input_check);
}

static void _bug82_pattern_clean(void);
static int _bug82_pattern(int part);
static pthread_mutex_t _bug82_mutex1;
static pthread_cond_t _bug82_cond1;
static volatile __attribute__((used)) int* volatile _bug82_ptr;
static volatile __attribute__((used)) int _bug82_val;
static volatile __attribute__((used)) int _bug82_pred;
static void _bug82_pattern_clean(void)
{
    pthread_mutex_init(&_bug82_mutex1, NULL);
    pthread_cond_init(&_bug82_cond1, NULL);
    _bug82_ptr = NULL;
    _bug82_val = 0;
    _bug82_pred = 0;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug82_pred = 0;
        pthread_cond_signal(&_bug82_cond1);
        pthread_mutex_lock(&_bug82_mutex1);
        _bug82_ptr = &_bug82_val;
        _bug82_pred = 1;
        pthread_mutex_unlock(&_bug82_mutex1);
        pthread_cond_signal(&_bug82_cond1);
    }
    else {
        pthread_mutex_lock(&_bug82_mutex1);
        if (!_bug82_pred)
            pthread_cond_wait(&_bug82_cond1, &_bug82_mutex1);
        if (_bug82_ptr == NULL)
            trigger = 1;
        _bug82_ptr = NULL;
        _bug82_pred = 0;
        pthread_mutex_unlock(&_bug82_mutex1);
    }
    return trigger;
}

static void _bug82_pattern_clean(void);
static int _bug82_pattern(int part);
static racebench_bug_helper _bug82_bug_helper;
static racebench_input_helper _bug82_input_helper;
__attribute__((constructor))
static void _bug82_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1c8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xd551ef7b};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1189c4e};
    static racebench_before_helper before_helpers[2] = {};
    _bug82_input_helper.interleave_num = 2;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0x766a8bc9;
}
static int _bug82_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[82], 1);
        _bug82_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug82_pattern(part);
}
static int _bug82_input_check(int part)
{
    return rb_input_check(&_bug82_input_helper, 82, part, _bug82_bug_callback);
}
void racebench_bug82(int part)
{
    rb_bug_synchronize(&_bug82_bug_helper, 82, part, _bug82_input_check);
}

static void _bug83_pattern_clean(void);
static int _bug83_pattern(int part);
static pthread_mutex_t _bug83_mutex1;
static volatile __attribute__((used)) int* volatile _bug83_ptr;
static volatile __attribute__((used)) int _bug83_val;
static void _bug83_pattern_clean(void)
{
    pthread_mutex_init(&_bug83_mutex1, NULL);
    _bug83_ptr = NULL;
    _bug83_val = 0;
}
static __attribute__((optimize(0))) int _bug83_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug83_mutex1);
        _bug83_ptr = &_bug83_val;
        pthread_mutex_unlock(&_bug83_mutex1);
        pthread_mutex_lock(&_bug83_mutex1);
        if (_bug83_ptr == NULL)
            trigger = 1;
        _bug83_ptr = NULL;
        pthread_mutex_unlock(&_bug83_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug83_mutex1);
        _bug83_ptr = &_bug83_val;
        if (_bug83_ptr == NULL)
            trigger = 1;
        _bug83_ptr = NULL;
        pthread_mutex_unlock(&_bug83_mutex1);
    }
    return trigger;
}

static void _bug83_pattern_clean(void);
static int _bug83_pattern(int part);
static racebench_bug_helper _bug83_bug_helper;
static racebench_input_helper _bug83_input_helper;
__attribute__((constructor))
static void _bug83_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182ebe};
    static racebench_before_helper before_helpers[1] = {};
    _bug83_input_helper.interleave_num = 1;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0xa1182ebe;
}
static int _bug83_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[83], 1);
        _bug83_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug83_pattern(part);
}
static int _bug83_input_check(int part)
{
    return rb_input_check(&_bug83_input_helper, 83, part, _bug83_bug_callback);
}
void racebench_bug83(int part)
{
    rb_bug_synchronize(&_bug83_bug_helper, 83, part, _bug83_input_check);
}

static void _bug84_pattern_clean(void);
static int _bug84_pattern(int part);
static pthread_mutex_t _bug84_mutex1;
static pthread_cond_t _bug84_cond1;
static volatile __attribute__((used)) int _bug84_pred;
static volatile __attribute__((used)) int _bug84_sent;
static void _bug84_pattern_clean(void)
{
    pthread_mutex_init(&_bug84_mutex1, NULL);
    pthread_cond_init(&_bug84_cond1, NULL);
    _bug84_pred = 0;
    _bug84_sent = 0;
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug84_mutex1);
        _bug84_pred = 1;
        pthread_cond_signal(&_bug84_cond1);
        _bug84_sent = 1;
        pthread_mutex_unlock(&_bug84_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug84_mutex1);
        if (_bug84_sent) {
            trigger = 1;
        }
        else {
            while (_bug84_pred == 0)
                pthread_cond_wait(&_bug84_cond1, &_bug84_mutex1);
        }
        _bug84_pred = 0;
        pthread_mutex_unlock(&_bug84_mutex1);
    }
    return trigger;
}

static void _bug84_pattern_clean(void);
static int _bug84_pattern(int part);
static racebench_bug_helper _bug84_bug_helper;
static racebench_input_helper _bug84_input_helper;
__attribute__((constructor))
static void _bug84_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1ef};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa114cf9e};
    static racebench_before_helper before_helpers[1] = {};
    _bug84_input_helper.interleave_num = 1;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xa114cf9e;
}
static int _bug84_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[84], 1);
        _bug84_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug84_pattern(part);
}
static int _bug84_input_check(int part)
{
    return rb_input_check(&_bug84_input_helper, 84, part, _bug84_bug_callback);
}
void racebench_bug84(int part)
{
    rb_bug_synchronize(&_bug84_bug_helper, 84, part, _bug84_input_check);
}

static void _bug85_pattern_clean(void);
static int _bug85_pattern(int part);
static pthread_mutex_t _bug85_mutex1;
static volatile __attribute__((used)) int _bug85_first_part;
static void _bug85_pattern_clean(void)
{
    pthread_mutex_init(&_bug85_mutex1, NULL);
    _bug85_first_part = -1;
}
static __attribute__((optimize(0))) int _bug85_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug85_mutex1);
        if (_bug85_first_part == -1)
            _bug85_first_part = part;
        else
            _bug85_first_part = -1;
        pthread_mutex_unlock(&_bug85_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug85_mutex1);
        if (_bug85_first_part == -1) {
            _bug85_first_part = part;
            trigger = 1;
        }
        else
            _bug85_first_part = -1;
        pthread_mutex_unlock(&_bug85_mutex1);
    }
    return trigger;
}

static void _bug85_pattern_clean(void);
static int _bug85_pattern(int part);
static racebench_bug_helper _bug85_bug_helper;
static racebench_input_helper _bug85_input_helper;
__attribute__((constructor))
static void _bug85_input_helper_init(void)
{
    static const uint32_t indices[] = {0x16};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa2c9d087};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x86da700f};
    static racebench_before_helper before_helpers[2] = {};
    _bug85_input_helper.interleave_num = 2;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0x29a44096;
}
static int _bug85_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[85], 1);
        _bug85_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug85_pattern(part);
}
static int _bug85_input_check(int part)
{
    return rb_input_check(&_bug85_input_helper, 85, part, _bug85_bug_callback);
}
void racebench_bug85(int part)
{
    rb_bug_synchronize(&_bug85_bug_helper, 85, part, _bug85_input_check);
}

static void _bug86_pattern_clean(void);
static int _bug86_pattern(int part);
static volatile __attribute__((used)) int _bug86_arr[3];
static volatile __attribute__((used)) int _bug86_val;
static volatile __attribute__((used)) int _bug86_pred;
static void _bug86_pattern_clean(void)
{
    _bug86_arr[0] = 0, _bug86_arr[1] = 2, _bug86_arr[2] = 1;
    _bug86_val = 0;
    _bug86_pred = 0;
}
static __attribute__((optimize(0))) int _bug86_pattern(int part)
{
    int trigger = 0;
    _bug86_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug86_val += _bug86_arr[i];
        }
        __sync_synchronize();
        while (!_bug86_pred)
            pthread_yield();
        trigger = _bug86_val != 3;
        _bug86_pred = 0;
        _bug86_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug86_val += _bug86_arr[i];
        }
        __sync_synchronize();
        _bug86_pred = 1;
    }
    return trigger;
}

static void _bug86_pattern_clean(void);
static int _bug86_pattern(int part);
static racebench_bug_helper _bug86_bug_helper;
static racebench_input_helper _bug86_input_helper;
__attribute__((constructor))
static void _bug86_input_helper_init(void)
{
    static const uint32_t indices[] = {0x232};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x9f2eec6, 0x8b5c27c7};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11a2d5e};
    static racebench_before_helper before_helpers[3] = {};
    _bug86_input_helper.interleave_num = 3;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x366943eb;
}
static int _bug86_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[86], 1);
        _bug86_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug86_pattern(part);
}
static int _bug86_input_check(int part)
{
    return rb_input_check(&_bug86_input_helper, 86, part, _bug86_bug_callback);
}
void racebench_bug86(int part)
{
    rb_bug_synchronize(&_bug86_bug_helper, 86, part, _bug86_input_check);
}

static void _bug87_pattern_clean(void);
static int _bug87_pattern(int part);
static pthread_mutex_t _bug87_mutex1;
static volatile __attribute__((used)) int* volatile _bug87_ptr;
static volatile __attribute__((used)) int _bug87_val;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    _bug87_ptr = NULL;
    _bug87_val = 0;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug87_mutex1);
        _bug87_ptr = &_bug87_val;
        pthread_mutex_unlock(&_bug87_mutex1);
        pthread_mutex_lock(&_bug87_mutex1);
        if (_bug87_ptr == NULL)
            trigger = 1;
        _bug87_ptr = NULL;
        pthread_mutex_unlock(&_bug87_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug87_mutex1);
        _bug87_ptr = &_bug87_val;
        if (_bug87_ptr == NULL)
            trigger = 1;
        _bug87_ptr = NULL;
        pthread_mutex_unlock(&_bug87_mutex1);
    }
    return trigger;
}

static void _bug87_pattern_clean(void);
static int _bug87_pattern(int part);
static racebench_bug_helper _bug87_bug_helper;
static racebench_input_helper _bug87_input_helper;
__attribute__((constructor))
static void _bug87_input_helper_init(void)
{
    static const uint32_t indices[] = {0xf};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8169e789};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11826de};
    static racebench_before_helper before_helpers[2] = {};
    _bug87_input_helper.interleave_num = 2;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0x22820e67;
}
static int _bug87_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[87], 1);
        _bug87_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug87_pattern(part);
}
static int _bug87_input_check(int part)
{
    return rb_input_check(&_bug87_input_helper, 87, part, _bug87_bug_callback);
}
void racebench_bug87(int part)
{
    rb_bug_synchronize(&_bug87_bug_helper, 87, part, _bug87_input_check);
}

static void _bug88_pattern_clean(void);
static int _bug88_pattern(int part);
static pthread_mutex_t _bug88_mutex1;
static volatile __attribute__((used)) int _bug88_first_part;
static volatile __attribute__((used)) int _bug88_released;
static void _bug88_pattern_clean(void)
{
    pthread_mutex_init(&_bug88_mutex1, NULL);
    _bug88_first_part = -1;
    _bug88_released = -1;
}
static __attribute__((optimize(0))) int _bug88_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug88_mutex1) == 0)
                break;
            if (_bug88_released == 0) {
                _bug88_first_part = -1;
                _bug88_released = -1;
                return 1;
            }
        }
        if (_bug88_first_part == -1)
            _bug88_first_part = part;
        else {
            _bug88_first_part = -1;
            while (_bug88_released == -1) {}
            _bug88_released = -1;
        }
        pthread_mutex_unlock(&_bug88_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug88_mutex1);
        if (_bug88_first_part == -1) {
            _bug88_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug88_mutex1);
                _bug88_released = 1;
            }
            else {
                _bug88_released = 0;
                while (_bug88_released != -1) {}
                pthread_mutex_unlock(&_bug88_mutex1);
                return 1;
            }
        }
        else {
            _bug88_first_part = -1;
            pthread_mutex_unlock(&_bug88_mutex1);
        }
    }
    return 0;
}

static void _bug88_pattern_clean(void);
static int _bug88_pattern(int part);
static racebench_bug_helper _bug88_bug_helper;
static racebench_input_helper _bug88_input_helper;
__attribute__((constructor))
static void _bug88_input_helper_init(void)
{
    static const uint32_t indices[] = {0xd1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x792ed611};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118303e};
    static racebench_before_helper before_helpers[2] = {};
    _bug88_input_helper.interleave_num = 2;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0x1a47064f;
}
static int _bug88_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[88], 1);
        _bug88_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug88_pattern(part);
}
static int _bug88_input_check(int part)
{
    return rb_input_check(&_bug88_input_helper, 88, part, _bug88_bug_callback);
}
void racebench_bug88(int part)
{
    rb_bug_synchronize(&_bug88_bug_helper, 88, part, _bug88_input_check);
}

static void _bug89_pattern_clean(void);
static int _bug89_pattern(int part);
static pthread_mutex_t _bug89_mutex1;
static pthread_cond_t _bug89_cond1;
static volatile __attribute__((used)) int* volatile _bug89_ptr;
static volatile __attribute__((used)) int _bug89_val;
static volatile __attribute__((used)) int _bug89_pred;
static void _bug89_pattern_clean(void)
{
    pthread_mutex_init(&_bug89_mutex1, NULL);
    pthread_cond_init(&_bug89_cond1, NULL);
    _bug89_ptr = NULL;
    _bug89_val = 0;
    _bug89_pred = 0;
}
static __attribute__((optimize(0))) int _bug89_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug89_pred = 0;
        pthread_cond_signal(&_bug89_cond1);
        pthread_mutex_lock(&_bug89_mutex1);
        _bug89_ptr = &_bug89_val;
        _bug89_pred = 1;
        pthread_mutex_unlock(&_bug89_mutex1);
        pthread_cond_signal(&_bug89_cond1);
    }
    else {
        pthread_mutex_lock(&_bug89_mutex1);
        if (!_bug89_pred)
            pthread_cond_wait(&_bug89_cond1, &_bug89_mutex1);
        if (_bug89_ptr == NULL)
            trigger = 1;
        _bug89_ptr = NULL;
        _bug89_pred = 0;
        pthread_mutex_unlock(&_bug89_mutex1);
    }
    return trigger;
}

static void _bug89_pattern_clean(void);
static int _bug89_pattern(int part);
static racebench_bug_helper _bug89_bug_helper;
static racebench_input_helper _bug89_input_helper;
__attribute__((constructor))
static void _bug89_input_helper_init(void)
{
    static const uint32_t indices[] = {0xcc};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f7e};
    static racebench_before_helper before_helpers[1] = {};
    _bug89_input_helper.interleave_num = 1;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0xa1182f7e;
}
static int _bug89_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[89], 1);
        _bug89_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug89_pattern(part);
}
static int _bug89_input_check(int part)
{
    return rb_input_check(&_bug89_input_helper, 89, part, _bug89_bug_callback);
}
void racebench_bug89(int part)
{
    rb_bug_synchronize(&_bug89_bug_helper, 89, part, _bug89_input_check);
}

static void _bug90_pattern_clean(void);
static int _bug90_pattern(int part);
static volatile __attribute__((used)) int _bug90_ok;
static volatile __attribute__((used)) int* volatile _bug90_ptr;
static volatile __attribute__((used)) int _bug90_val;
static void _bug90_pattern_clean(void)
{
    _bug90_ok = 0;
    _bug90_ptr = NULL;
    _bug90_val = 0;
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug90_ok = 1;
        __sync_synchronize();
        _bug90_ptr = &_bug90_val;
    }
    else {
        while (!_bug90_ok)
            pthread_yield();
        if (_bug90_ptr == NULL)
            trigger = 1;
        _bug90_ptr = NULL;
        _bug90_ok = 0;
    }
    return trigger;
}

static void _bug90_pattern_clean(void);
static int _bug90_pattern(int part);
static racebench_bug_helper _bug90_bug_helper;
static racebench_input_helper _bug90_input_helper;
__attribute__((constructor))
static void _bug90_input_helper_init(void)
{
    static const uint32_t indices[] = {0x8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x9885df52};
    static racebench_before_helper before_helpers[0] = {};
    _bug90_input_helper.interleave_num = 0;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0x9885df52;
}
static int _bug90_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[90], 1);
        _bug90_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug90_pattern(part);
}
static int _bug90_input_check(int part)
{
    return rb_input_check(&_bug90_input_helper, 90, part, _bug90_bug_callback);
}
void racebench_bug90(int part)
{
    rb_bug_synchronize(&_bug90_bug_helper, 90, part, _bug90_input_check);
}

static void _bug91_pattern_clean(void);
static int _bug91_pattern(int part);
static pthread_mutex_t _bug91_mutex1;
static pthread_cond_t _bug91_cond1;
static volatile __attribute__((used)) int* volatile _bug91_ptr;
static volatile __attribute__((used)) int _bug91_val;
static volatile __attribute__((used)) int _bug91_pred;
static void _bug91_pattern_clean(void)
{
    pthread_mutex_init(&_bug91_mutex1, NULL);
    pthread_cond_init(&_bug91_cond1, NULL);
    _bug91_ptr = NULL;
    _bug91_val = 0;
    _bug91_pred = 0;
}
static __attribute__((optimize(0))) int _bug91_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug91_pred = 0;
        pthread_cond_signal(&_bug91_cond1);
        pthread_mutex_lock(&_bug91_mutex1);
        _bug91_ptr = &_bug91_val;
        _bug91_pred = 1;
        pthread_mutex_unlock(&_bug91_mutex1);
        pthread_cond_signal(&_bug91_cond1);
    }
    else {
        pthread_mutex_lock(&_bug91_mutex1);
        if (!_bug91_pred)
            pthread_cond_wait(&_bug91_cond1, &_bug91_mutex1);
        if (_bug91_ptr == NULL)
            trigger = 1;
        _bug91_ptr = NULL;
        _bug91_pred = 0;
        pthread_mutex_unlock(&_bug91_mutex1);
    }
    return trigger;
}

static void _bug91_pattern_clean(void);
static int _bug91_pattern(int part);
static racebench_bug_helper _bug91_bug_helper;
static racebench_input_helper _bug91_input_helper;
__attribute__((constructor))
static void _bug91_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1e5};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2be72e8c, 0xcb7e49c7};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11c2dae};
    static racebench_before_helper before_helpers[3] = {};
    _bug91_input_helper.interleave_num = 3;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0x9881a601;
}
static int _bug91_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[91], 1);
        _bug91_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug91_pattern(part);
}
static int _bug91_input_check(int part)
{
    return rb_input_check(&_bug91_input_helper, 91, part, _bug91_bug_callback);
}
void racebench_bug91(int part)
{
    rb_bug_synchronize(&_bug91_bug_helper, 91, part, _bug91_input_check);
}

static void _bug92_pattern_clean(void);
static int _bug92_pattern(int part);
static volatile __attribute__((used)) int _bug92_ok;
static volatile __attribute__((used)) int* volatile _bug92_ptr;
static volatile __attribute__((used)) int _bug92_val;
static void _bug92_pattern_clean(void)
{
    _bug92_ok = 0;
    _bug92_ptr = NULL;
    _bug92_val = 0;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug92_ok = 1;
        __sync_synchronize();
        _bug92_ptr = &_bug92_val;
    }
    else {
        while (!_bug92_ok)
            pthread_yield();
        if (_bug92_ptr == NULL)
            trigger = 1;
        _bug92_ptr = NULL;
        _bug92_ok = 0;
    }
    return trigger;
}

static void _bug92_pattern_clean(void);
static int _bug92_pattern(int part);
static racebench_bug_helper _bug92_bug_helper;
static racebench_input_helper _bug92_input_helper;
__attribute__((constructor))
static void _bug92_input_helper_init(void)
{
    static const uint32_t indices[] = {0xb2};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa111171e};
    static racebench_before_helper before_helpers[1] = {};
    _bug92_input_helper.interleave_num = 1;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0xa111171e;
}
static int _bug92_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[92], 1);
        _bug92_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug92_pattern(part);
}
static int _bug92_input_check(int part)
{
    return rb_input_check(&_bug92_input_helper, 92, part, _bug92_bug_callback);
}
void racebench_bug92(int part)
{
    rb_bug_synchronize(&_bug92_bug_helper, 92, part, _bug92_input_check);
}

static void _bug93_pattern_clean(void);
static int _bug93_pattern(int part);
static volatile __attribute__((used)) int _bug93_ok;
static volatile __attribute__((used)) int* volatile _bug93_ptr;
static volatile __attribute__((used)) int _bug93_val;
static void _bug93_pattern_clean(void)
{
    _bug93_ok = 0;
    _bug93_ptr = NULL;
    _bug93_val = 0;
}
static __attribute__((optimize(0))) int _bug93_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug93_ok = 1;
        __sync_synchronize();
        _bug93_ptr = &_bug93_val;
    }
    else {
        while (!_bug93_ok)
            pthread_yield();
        if (_bug93_ptr == NULL)
            trigger = 1;
        _bug93_ptr = NULL;
        _bug93_ok = 0;
    }
    return trigger;
}

static void _bug93_pattern_clean(void);
static int _bug93_pattern(int part);
static racebench_bug_helper _bug93_bug_helper;
static racebench_input_helper _bug93_input_helper;
__attribute__((constructor))
static void _bug93_input_helper_init(void)
{
    static const uint32_t indices[] = {0x232};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x521bb6c};
    static racebench_before_helper before_helpers[0] = {};
    _bug93_input_helper.interleave_num = 0;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0x521bb6c;
}
static int _bug93_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[93], 1);
        _bug93_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug93_pattern(part);
}
static int _bug93_input_check(int part)
{
    return rb_input_check(&_bug93_input_helper, 93, part, _bug93_bug_callback);
}
void racebench_bug93(int part)
{
    rb_bug_synchronize(&_bug93_bug_helper, 93, part, _bug93_input_check);
}

static void _bug94_pattern_clean(void);
static int _bug94_pattern(int part);
static volatile __attribute__((used)) int _bug94_arr[3];
static volatile __attribute__((used)) int _bug94_val;
static volatile __attribute__((used)) int _bug94_pred;
static void _bug94_pattern_clean(void)
{
    _bug94_arr[0] = 0, _bug94_arr[1] = 2, _bug94_arr[2] = 1;
    _bug94_val = 0;
    _bug94_pred = 0;
}
static __attribute__((optimize(0))) int _bug94_pattern(int part)
{
    int trigger = 0;
    _bug94_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug94_val += _bug94_arr[i];
        }
        __sync_synchronize();
        while (!_bug94_pred)
            pthread_yield();
        trigger = _bug94_val != 3;
        _bug94_pred = 0;
        _bug94_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug94_val += _bug94_arr[i];
        }
        __sync_synchronize();
        _bug94_pred = 1;
    }
    return trigger;
}

static void _bug94_pattern_clean(void);
static int _bug94_pattern(int part);
static racebench_bug_helper _bug94_bug_helper;
static racebench_input_helper _bug94_input_helper;
__attribute__((constructor))
static void _bug94_input_helper_init(void)
{
    static const uint32_t indices[] = {0x20a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1189c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug94_input_helper.interleave_num = 1;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0xa1189c4e;
}
static int _bug94_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[94], 1);
        _bug94_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug94_pattern(part);
}
static int _bug94_input_check(int part)
{
    return rb_input_check(&_bug94_input_helper, 94, part, _bug94_bug_callback);
}
void racebench_bug94(int part)
{
    rb_bug_synchronize(&_bug94_bug_helper, 94, part, _bug94_input_check);
}

static void _bug95_pattern_clean(void);
static int _bug95_pattern(int part);
static pthread_mutex_t _bug95_mutex1;
static volatile __attribute__((used)) int _bug95_first_part;
static volatile __attribute__((used)) int _bug95_released;
static void _bug95_pattern_clean(void)
{
    pthread_mutex_init(&_bug95_mutex1, NULL);
    _bug95_first_part = -1;
    _bug95_released = -1;
}
static __attribute__((optimize(0))) int _bug95_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug95_mutex1) == 0)
                break;
            if (_bug95_released == 0) {
                _bug95_first_part = -1;
                _bug95_released = -1;
                return 1;
            }
        }
        if (_bug95_first_part == -1)
            _bug95_first_part = part;
        else {
            _bug95_first_part = -1;
            while (_bug95_released == -1) {}
            _bug95_released = -1;
        }
        pthread_mutex_unlock(&_bug95_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug95_mutex1);
        if (_bug95_first_part == -1) {
            _bug95_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug95_mutex1);
                _bug95_released = 1;
            }
            else {
                _bug95_released = 0;
                while (_bug95_released != -1) {}
                pthread_mutex_unlock(&_bug95_mutex1);
                return 1;
            }
        }
        else {
            _bug95_first_part = -1;
            pthread_mutex_unlock(&_bug95_mutex1);
        }
    }
    return 0;
}

static void _bug95_pattern_clean(void);
static int _bug95_pattern(int part);
static racebench_bug_helper _bug95_bug_helper;
static racebench_input_helper _bug95_input_helper;
__attribute__((constructor))
static void _bug95_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xc0e5fc7f};
    static racebench_before_helper before_helpers[0] = {};
    _bug95_input_helper.interleave_num = 0;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xc0e5fc7f;
}
static int _bug95_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[95], 1);
        _bug95_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug95_pattern(part);
}
static int _bug95_input_check(int part)
{
    return rb_input_check(&_bug95_input_helper, 95, part, _bug95_bug_callback);
}
void racebench_bug95(int part)
{
    rb_bug_synchronize(&_bug95_bug_helper, 95, part, _bug95_input_check);
}

static void _bug96_pattern_clean(void);
static int _bug96_pattern(int part);
static pthread_mutex_t _bug96_mutex1;
static pthread_cond_t _bug96_cond1;
static volatile __attribute__((used)) int _bug96_pred;
static volatile __attribute__((used)) int _bug96_sent;
static void _bug96_pattern_clean(void)
{
    pthread_mutex_init(&_bug96_mutex1, NULL);
    pthread_cond_init(&_bug96_cond1, NULL);
    _bug96_pred = 0;
    _bug96_sent = 0;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug96_mutex1);
        _bug96_pred = 1;
        pthread_cond_signal(&_bug96_cond1);
        _bug96_sent = 1;
        pthread_mutex_unlock(&_bug96_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug96_mutex1);
        if (_bug96_sent) {
            trigger = 1;
        }
        else {
            while (_bug96_pred == 0)
                pthread_cond_wait(&_bug96_cond1, &_bug96_mutex1);
        }
        _bug96_pred = 0;
        pthread_mutex_unlock(&_bug96_mutex1);
    }
    return trigger;
}

static void _bug96_pattern_clean(void);
static int _bug96_pattern(int part);
static racebench_bug_helper _bug96_bug_helper;
static racebench_input_helper _bug96_input_helper;
__attribute__((constructor))
static void _bug96_input_helper_init(void)
{
    static const uint32_t indices[] = {0x23e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x5a5d41b7};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x98953c8f};
    static racebench_before_helper before_helpers[2] = {};
    _bug96_input_helper.interleave_num = 2;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xf2f27e46;
}
static int _bug96_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[96], 1);
        _bug96_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug96_pattern(part);
}
static int _bug96_input_check(int part)
{
    return rb_input_check(&_bug96_input_helper, 96, part, _bug96_bug_callback);
}
void racebench_bug96(int part)
{
    rb_bug_synchronize(&_bug96_bug_helper, 96, part, _bug96_input_check);
}

static void _bug97_pattern_clean(void);
static int _bug97_pattern(int part);
static pthread_mutex_t _bug97_mutex1;
static volatile __attribute__((used)) int _bug97_val;
static void _bug97_pattern_clean(void)
{
    pthread_mutex_init(&_bug97_mutex1, NULL);
    _bug97_val = 0;
}
static __attribute__((optimize(0))) int _bug97_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug97_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug97_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug97_mutex1);
    }
    else {
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
    }
    return trigger;
}

static void _bug97_pattern_clean(void);
static int _bug97_pattern(int part);
static racebench_bug_helper _bug97_bug_helper;
static racebench_input_helper _bug97_input_helper;
__attribute__((constructor))
static void _bug97_input_helper_init(void)
{
    static const uint32_t indices[] = {0xc};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xeb9dd57d, 0xfe2ded2f};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa112d17e};
    static racebench_before_helper before_helpers[3] = {};
    _bug97_input_helper.interleave_num = 3;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0x8ade942a;
}
static int _bug97_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[97], 1);
        _bug97_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug97_pattern(part);
}
static int _bug97_input_check(int part)
{
    return rb_input_check(&_bug97_input_helper, 97, part, _bug97_bug_callback);
}
void racebench_bug97(int part)
{
    rb_bug_synchronize(&_bug97_bug_helper, 97, part, _bug97_input_check);
}

static void _bug98_pattern_clean(void);
static int _bug98_pattern(int part);
static volatile __attribute__((used)) int _bug98_ok;
static volatile __attribute__((used)) int* volatile _bug98_ptr;
static volatile __attribute__((used)) int _bug98_val;
static void _bug98_pattern_clean(void)
{
    _bug98_ok = 0;
    _bug98_ptr = NULL;
    _bug98_val = 0;
}
static __attribute__((optimize(0))) int _bug98_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug98_ok = 1;
        __sync_synchronize();
        _bug98_ptr = &_bug98_val;
    }
    else {
        while (!_bug98_ok)
            pthread_yield();
        if (_bug98_ptr == NULL)
            trigger = 1;
        _bug98_ptr = NULL;
        _bug98_ok = 0;
    }
    return trigger;
}

static void _bug98_pattern_clean(void);
static int _bug98_pattern(int part);
static racebench_bug_helper _bug98_bug_helper;
static racebench_input_helper _bug98_input_helper;
__attribute__((constructor))
static void _bug98_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x56c81699};
    static racebench_before_helper before_helpers[1] = {};
    _bug98_input_helper.interleave_num = 1;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0x56c81699;
}
static int _bug98_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[98], 1);
        _bug98_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug98_pattern(part);
}
static int _bug98_input_check(int part)
{
    return rb_input_check(&_bug98_input_helper, 98, part, _bug98_bug_callback);
}
void racebench_bug98(int part)
{
    rb_bug_synchronize(&_bug98_bug_helper, 98, part, _bug98_input_check);
}

static void _bug99_pattern_clean(void);
static int _bug99_pattern(int part);
static pthread_mutex_t _bug99_mutex1;
static pthread_mutex_t _bug99_mutex2;
static void _bug99_pattern_clean(void)
{
    pthread_mutex_init(&_bug99_mutex1, NULL);
    pthread_mutex_init(&_bug99_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug99_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug99_mutex1);
        if (pthread_mutex_trylock(&_bug99_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug99_mutex2);
        pthread_mutex_unlock(&_bug99_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug99_mutex2);
        if (pthread_mutex_trylock(&_bug99_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug99_mutex1);
        pthread_mutex_unlock(&_bug99_mutex2);
    }
    return trigger;
}

static void _bug99_pattern_clean(void);
static int _bug99_pattern(int part);
static racebench_bug_helper _bug99_bug_helper;
static racebench_input_helper _bug99_input_helper;
__attribute__((constructor))
static void _bug99_input_helper_init(void)
{
    static const uint32_t indices[] = {0x23d};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x354fef33, 0xfa912e1d};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f9e};
    static racebench_before_helper before_helpers[3] = {};
    _bug99_input_helper.interleave_num = 3;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0xd0f94cee;
}
static int _bug99_bug_callback(int part)
{
    static racebench_barrier_helper barrier = {};
    if (part == 0) {
        __sync_fetch_and_add(&rb_stat.check_pass_num[99], 1);
        _bug99_pattern_clean();
    }
    rb_barrier(&barrier);
    return _bug99_pattern(part);
}
static int _bug99_input_check(int part)
{
    return rb_input_check(&_bug99_input_helper, 99, part, _bug99_bug_callback);
}
void racebench_bug99(int part)
{
    rb_bug_synchronize(&_bug99_bug_helper, 99, part, _bug99_input_check);
}
