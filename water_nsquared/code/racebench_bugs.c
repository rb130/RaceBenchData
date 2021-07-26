#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "racebench.h"
static void _bug0_pattern_clean(void);
static int _bug0_pattern(int part);
static pthread_mutex_t _bug0_mutex1;
static volatile __attribute__((used)) int* volatile _bug0_ptr;
static volatile __attribute__((used)) int _bug0_val;
static void _bug0_pattern_clean(void)
{
    pthread_mutex_init(&_bug0_mutex1, NULL);
    _bug0_ptr = NULL;
    _bug0_val = 0;
}
static __attribute__((optimize(0))) int _bug0_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug0_mutex1);
        _bug0_ptr = &_bug0_val;
        pthread_mutex_unlock(&_bug0_mutex1);
        pthread_mutex_lock(&_bug0_mutex1);
        if (_bug0_ptr == NULL)
            trigger = 1;
        _bug0_ptr = NULL;
        pthread_mutex_unlock(&_bug0_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug0_mutex1);
        _bug0_ptr = &_bug0_val;
        if (_bug0_ptr == NULL)
            trigger = 1;
        _bug0_ptr = NULL;
        pthread_mutex_unlock(&_bug0_mutex1);
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
    static const uint32_t indices[] = {0x2b902f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x49b45d46};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x84252479};
    static racebench_before_helper before_helpers[2] = {};
    _bug0_input_helper.interleave_num = 2;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0xcdd981bf;
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
static volatile __attribute__((used)) int* volatile _bug1_ptr;
static volatile __attribute__((used)) int _bug1_val;
static void _bug1_pattern_clean(void)
{
    pthread_mutex_init(&_bug1_mutex1, NULL);
    _bug1_ptr = NULL;
    _bug1_val = 0;
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug1_mutex1);
        _bug1_ptr = &_bug1_val;
        pthread_mutex_unlock(&_bug1_mutex1);
        pthread_mutex_lock(&_bug1_mutex1);
        if (_bug1_ptr == NULL)
            trigger = 1;
        _bug1_ptr = NULL;
        pthread_mutex_unlock(&_bug1_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug1_mutex1);
        _bug1_ptr = &_bug1_val;
        if (_bug1_ptr == NULL)
            trigger = 1;
        _bug1_ptr = NULL;
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
    static const uint32_t indices[] = {0x25c1b5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xd7554b7c};
    static racebench_before_helper before_helpers[1] = {};
    _bug1_input_helper.interleave_num = 1;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xd7554b7c;
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
static pthread_mutex_t _bug2_mutex1;
static pthread_cond_t _bug2_cond1;
static volatile __attribute__((used)) int* volatile _bug2_ptr;
static volatile __attribute__((used)) int _bug2_val;
static volatile __attribute__((used)) int _bug2_pred;
static void _bug2_pattern_clean(void)
{
    pthread_mutex_init(&_bug2_mutex1, NULL);
    pthread_cond_init(&_bug2_cond1, NULL);
    _bug2_ptr = NULL;
    _bug2_val = 0;
    _bug2_pred = 0;
}
static __attribute__((optimize(0))) int _bug2_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug2_pred = 0;
        pthread_cond_signal(&_bug2_cond1);
        pthread_mutex_lock(&_bug2_mutex1);
        _bug2_ptr = &_bug2_val;
        _bug2_pred = 1;
        pthread_mutex_unlock(&_bug2_mutex1);
        pthread_cond_signal(&_bug2_cond1);
    }
    else {
        pthread_mutex_lock(&_bug2_mutex1);
        if (!_bug2_pred)
            pthread_cond_wait(&_bug2_cond1, &_bug2_mutex1);
        if (_bug2_ptr == NULL)
            trigger = 1;
        _bug2_ptr = NULL;
        _bug2_pred = 0;
        pthread_mutex_unlock(&_bug2_mutex1);
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
    static const uint32_t indices[] = {0x13e23e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x2a4ff16e, 0x42ebb7ed};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x63068df0};
    static racebench_before_helper before_helpers[3] = {};
    _bug2_input_helper.interleave_num = 3;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0xd042374b;
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
    static const uint32_t indices[] = {0x11715c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa11290fe};
    static racebench_before_helper before_helpers[1] = {};
    _bug3_input_helper.interleave_num = 1;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xa11290fe;
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
static volatile __attribute__((used)) int _bug4_first_part;
static volatile __attribute__((used)) int _bug4_released;
static void _bug4_pattern_clean(void)
{
    pthread_mutex_init(&_bug4_mutex1, NULL);
    _bug4_first_part = -1;
    _bug4_released = -1;
}
static __attribute__((optimize(0))) int _bug4_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug4_mutex1) == 0)
                break;
            if (_bug4_released == 0) {
                _bug4_first_part = -1;
                _bug4_released = -1;
                return 1;
            }
        }
        if (_bug4_first_part == -1)
            _bug4_first_part = part;
        else {
            _bug4_first_part = -1;
            while (_bug4_released == -1) {}
            _bug4_released = -1;
        }
        pthread_mutex_unlock(&_bug4_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug4_mutex1);
        if (_bug4_first_part == -1) {
            _bug4_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug4_mutex1);
                _bug4_released = 1;
            }
            else {
                _bug4_released = 0;
                while (_bug4_released != -1) {}
                pthread_mutex_unlock(&_bug4_mutex1);
                return 1;
            }
        }
        else {
            _bug4_first_part = -1;
            pthread_mutex_unlock(&_bug4_mutex1);
        }
    }
    return 0;
}

static void _bug4_pattern_clean(void);
static int _bug4_pattern(int part);
static racebench_bug_helper _bug4_bug_helper;
static racebench_input_helper _bug4_input_helper;
__attribute__((constructor))
static void _bug4_input_helper_init(void)
{
    static const uint32_t indices[] = {0x8a688};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xbc1f32ac};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[2] = {};
    _bug4_input_helper.interleave_num = 2;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0x5d375f3a;
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
static pthread_cond_t _bug5_cond1;
static volatile __attribute__((used)) int* volatile _bug5_ptr;
static volatile __attribute__((used)) int _bug5_val;
static volatile __attribute__((used)) int _bug5_pred;
static void _bug5_pattern_clean(void)
{
    pthread_mutex_init(&_bug5_mutex1, NULL);
    pthread_cond_init(&_bug5_cond1, NULL);
    _bug5_ptr = NULL;
    _bug5_val = 0;
    _bug5_pred = 0;
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug5_pred = 0;
        pthread_cond_signal(&_bug5_cond1);
        pthread_mutex_lock(&_bug5_mutex1);
        _bug5_ptr = &_bug5_val;
        _bug5_pred = 1;
        pthread_mutex_unlock(&_bug5_mutex1);
        pthread_cond_signal(&_bug5_cond1);
    }
    else {
        pthread_mutex_lock(&_bug5_mutex1);
        if (!_bug5_pred)
            pthread_cond_wait(&_bug5_cond1, &_bug5_mutex1);
        if (_bug5_ptr == NULL)
            trigger = 1;
        _bug5_ptr = NULL;
        _bug5_pred = 0;
        pthread_mutex_unlock(&_bug5_mutex1);
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
    static const uint32_t indices[] = {0x2d798};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xd4837caf};
    static racebench_before_helper before_helpers[0] = {};
    _bug5_input_helper.interleave_num = 0;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xd4837caf;
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
static volatile __attribute__((used)) int _bug6_ok;
static volatile __attribute__((used)) int* volatile _bug6_ptr;
static volatile __attribute__((used)) int _bug6_val;
static void _bug6_pattern_clean(void)
{
    _bug6_ok = 0;
    _bug6_ptr = NULL;
    _bug6_val = 0;
}
static __attribute__((optimize(0))) int _bug6_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug6_ok = 1;
        __sync_synchronize();
        _bug6_ptr = &_bug6_val;
    }
    else {
        while (!_bug6_ok)
            pthread_yield();
        if (_bug6_ptr == NULL)
            trigger = 1;
        _bug6_ptr = NULL;
        _bug6_ok = 0;
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
    static const uint32_t indices[] = {0x1b4d6f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xefd3449, 0x7a4a5d3a};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xf8f4d1b7};
    static racebench_before_helper before_helpers[3] = {};
    _bug6_input_helper.interleave_num = 3;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0x823c633a;
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
static pthread_mutex_t _bug7_mutex2;
static void _bug7_pattern_clean(void)
{
    pthread_mutex_init(&_bug7_mutex1, NULL);
    pthread_mutex_init(&_bug7_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug7_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug7_mutex1);
        if (pthread_mutex_trylock(&_bug7_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug7_mutex2);
        pthread_mutex_unlock(&_bug7_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug7_mutex2);
        if (pthread_mutex_trylock(&_bug7_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug7_mutex1);
        pthread_mutex_unlock(&_bug7_mutex2);
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
    static const uint32_t indices[] = {0x352a94};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x758fa87};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cde};
    static racebench_before_helper before_helpers[2] = {};
    _bug7_input_helper.interleave_num = 2;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xa8712765;
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
static volatile __attribute__((used)) int _bug8_arr[3];
static volatile __attribute__((used)) int _bug8_val;
static volatile __attribute__((used)) int _bug8_pred;
static void _bug8_pattern_clean(void)
{
    _bug8_arr[0] = 0, _bug8_arr[1] = 2, _bug8_arr[2] = 1;
    _bug8_val = 0;
    _bug8_pred = 0;
}
static __attribute__((optimize(0))) int _bug8_pattern(int part)
{
    int trigger = 0;
    _bug8_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug8_val += _bug8_arr[i];
        }
        __sync_synchronize();
        while (!_bug8_pred)
            pthread_yield();
        trigger = _bug8_val != 3;
        _bug8_pred = 0;
        _bug8_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug8_val += _bug8_arr[i];
        }
        __sync_synchronize();
        _bug8_pred = 1;
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
    static const uint32_t indices[] = {0x349142};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x33e5b02d};
    static racebench_before_helper before_helpers[0] = {};
    _bug8_input_helper.interleave_num = 0;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0x33e5b02d;
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
static volatile __attribute__((used)) int _bug9_first_part;
static volatile __attribute__((used)) int _bug9_released;
static void _bug9_pattern_clean(void)
{
    pthread_mutex_init(&_bug9_mutex1, NULL);
    _bug9_first_part = -1;
    _bug9_released = -1;
}
static __attribute__((optimize(0))) int _bug9_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug9_mutex1) == 0)
                break;
            if (_bug9_released == 0) {
                _bug9_first_part = -1;
                _bug9_released = -1;
                return 1;
            }
        }
        if (_bug9_first_part == -1)
            _bug9_first_part = part;
        else {
            _bug9_first_part = -1;
            while (_bug9_released == -1) {}
            _bug9_released = -1;
        }
        pthread_mutex_unlock(&_bug9_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug9_mutex1);
        if (_bug9_first_part == -1) {
            _bug9_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug9_mutex1);
                _bug9_released = 1;
            }
            else {
                _bug9_released = 0;
                while (_bug9_released != -1) {}
                pthread_mutex_unlock(&_bug9_mutex1);
                return 1;
            }
        }
        else {
            _bug9_first_part = -1;
            pthread_mutex_unlock(&_bug9_mutex1);
        }
    }
    return 0;
}

static void _bug9_pattern_clean(void);
static int _bug9_pattern(int part);
static racebench_bug_helper _bug9_bug_helper;
static racebench_input_helper _bug9_input_helper;
__attribute__((constructor))
static void _bug9_input_helper_init(void)
{
    static const uint32_t indices[] = {0x25b0b8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xefb5f976};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x43942db9};
    static racebench_before_helper before_helpers[2] = {};
    _bug9_input_helper.interleave_num = 2;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0x334a272f;
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
    static const uint32_t indices[] = {0x3a2de};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118306e};
    static racebench_before_helper before_helpers[0] = {};
    _bug10_input_helper.interleave_num = 0;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0xa118306e;
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
static volatile __attribute__((used)) int _bug11_first_part;
static void _bug11_pattern_clean(void)
{
    pthread_mutex_init(&_bug11_mutex1, NULL);
    _bug11_first_part = -1;
}
static __attribute__((optimize(0))) int _bug11_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug11_mutex1);
        if (_bug11_first_part == -1)
            _bug11_first_part = part;
        else
            _bug11_first_part = -1;
        pthread_mutex_unlock(&_bug11_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug11_mutex1);
        if (_bug11_first_part == -1) {
            _bug11_first_part = part;
            trigger = 1;
        }
        else
            _bug11_first_part = -1;
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
    static const uint32_t indices[] = {0x33864e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x20a93879};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xc83725dc};
    static racebench_before_helper before_helpers[2] = {};
    _bug11_input_helper.interleave_num = 2;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0xe8e05e55;
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
static pthread_mutex_t _bug12_mutex1;
static volatile __attribute__((used)) int _bug12_first_part;
static void _bug12_pattern_clean(void)
{
    pthread_mutex_init(&_bug12_mutex1, NULL);
    _bug12_first_part = -1;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug12_mutex1);
        if (_bug12_first_part == -1)
            _bug12_first_part = part;
        else
            _bug12_first_part = -1;
        pthread_mutex_unlock(&_bug12_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug12_mutex1);
        if (_bug12_first_part == -1) {
            _bug12_first_part = part;
            trigger = 1;
        }
        else
            _bug12_first_part = -1;
        pthread_mutex_unlock(&_bug12_mutex1);
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
    static const uint32_t indices[] = {0x27ee1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xa4db0102};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug12_input_helper.interleave_num = 2;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0x45f32d70;
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
    static const uint32_t indices[] = {0x1e0941};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2d38019d};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1166c2e};
    static racebench_before_helper before_helpers[2] = {};
    _bug13_input_helper.interleave_num = 2;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0xce4e6dcb;
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
static pthread_mutex_t _bug14_mutex1;
static volatile __attribute__((used)) int _bug14_val;
static void _bug14_pattern_clean(void)
{
    pthread_mutex_init(&_bug14_mutex1, NULL);
    _bug14_val = 0;
}
static __attribute__((optimize(0))) int _bug14_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug14_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug14_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug14_mutex1);
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

static void _bug14_pattern_clean(void);
static int _bug14_pattern(int part);
static racebench_bug_helper _bug14_bug_helper;
static racebench_input_helper _bug14_input_helper;
__attribute__((constructor))
static void _bug14_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2f8004};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cde};
    static racebench_before_helper before_helpers[1] = {};
    _bug14_input_helper.interleave_num = 1;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xa1182cde;
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
static volatile __attribute__((used)) int _bug15_arr[3];
static volatile __attribute__((used)) int _bug15_val;
static volatile __attribute__((used)) int _bug15_pred;
static void _bug15_pattern_clean(void)
{
    _bug15_arr[0] = 0, _bug15_arr[1] = 2, _bug15_arr[2] = 1;
    _bug15_val = 0;
    _bug15_pred = 0;
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    _bug15_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug15_val += _bug15_arr[i];
        }
        __sync_synchronize();
        while (!_bug15_pred)
            pthread_yield();
        trigger = _bug15_val != 3;
        _bug15_pred = 0;
        _bug15_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug15_val += _bug15_arr[i];
        }
        __sync_synchronize();
        _bug15_pred = 1;
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
    static const uint32_t indices[] = {0x1125e6};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e5e};
    static racebench_before_helper before_helpers[0] = {};
    _bug15_input_helper.interleave_num = 0;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0xa1182e5e;
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
static volatile __attribute__((used)) int _bug16_first_part;
static volatile __attribute__((used)) int _bug16_released;
static void _bug16_pattern_clean(void)
{
    pthread_mutex_init(&_bug16_mutex1, NULL);
    _bug16_first_part = -1;
    _bug16_released = -1;
}
static __attribute__((optimize(0))) int _bug16_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug16_mutex1) == 0)
                break;
            if (_bug16_released == 0) {
                _bug16_first_part = -1;
                _bug16_released = -1;
                return 1;
            }
        }
        if (_bug16_first_part == -1)
            _bug16_first_part = part;
        else {
            _bug16_first_part = -1;
            while (_bug16_released == -1) {}
            _bug16_released = -1;
        }
        pthread_mutex_unlock(&_bug16_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug16_mutex1);
        if (_bug16_first_part == -1) {
            _bug16_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug16_mutex1);
                _bug16_released = 1;
            }
            else {
                _bug16_released = 0;
                while (_bug16_released != -1) {}
                pthread_mutex_unlock(&_bug16_mutex1);
                return 1;
            }
        }
        else {
            _bug16_first_part = -1;
            pthread_mutex_unlock(&_bug16_mutex1);
        }
    }
    return 0;
}

static void _bug16_pattern_clean(void);
static int _bug16_pattern(int part);
static racebench_bug_helper _bug16_bug_helper;
static racebench_input_helper _bug16_input_helper;
__attribute__((constructor))
static void _bug16_input_helper_init(void)
{
    static const uint32_t indices[] = {0x304ea5};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1157cce};
    static racebench_before_helper before_helpers[0] = {};
    _bug16_input_helper.interleave_num = 0;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0xa1157cce;
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
static pthread_cond_t _bug17_cond1;
static volatile __attribute__((used)) int _bug17_pred;
static volatile __attribute__((used)) int _bug17_sent;
static void _bug17_pattern_clean(void)
{
    pthread_mutex_init(&_bug17_mutex1, NULL);
    pthread_cond_init(&_bug17_cond1, NULL);
    _bug17_pred = 0;
    _bug17_sent = 0;
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug17_mutex1);
        _bug17_pred = 1;
        pthread_cond_signal(&_bug17_cond1);
        _bug17_sent = 1;
        pthread_mutex_unlock(&_bug17_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug17_mutex1);
        if (_bug17_sent) {
            trigger = 1;
        }
        else {
            while (_bug17_pred == 0)
                pthread_cond_wait(&_bug17_cond1, &_bug17_mutex1);
        }
        _bug17_pred = 0;
        pthread_mutex_unlock(&_bug17_mutex1);
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
    static const uint32_t indices[] = {0x1f1ab1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182fae};
    static racebench_before_helper before_helpers[1] = {};
    _bug17_input_helper.interleave_num = 1;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0xa1182fae;
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
    static const uint32_t indices[] = {0x303d44};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd28ae37a};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xc5854bdb};
    static racebench_before_helper before_helpers[2] = {};
    _bug18_input_helper.interleave_num = 2;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0x98102f55;
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
static volatile __attribute__((used)) int _bug19_first_part;
static void _bug19_pattern_clean(void)
{
    pthread_mutex_init(&_bug19_mutex1, NULL);
    _bug19_first_part = -1;
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug19_mutex1);
        if (_bug19_first_part == -1)
            _bug19_first_part = part;
        else
            _bug19_first_part = -1;
        pthread_mutex_unlock(&_bug19_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug19_mutex1);
        if (_bug19_first_part == -1) {
            _bug19_first_part = part;
            trigger = 1;
        }
        else
            _bug19_first_part = -1;
        pthread_mutex_unlock(&_bug19_mutex1);
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
    static const uint32_t indices[] = {0x19e8b2};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x91b31762, 0xeb2685c3};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x92d19208};
    static racebench_before_helper before_helpers[3] = {};
    _bug19_input_helper.interleave_num = 3;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0xfab2f2d;
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
static pthread_mutex_t _bug20_mutex1;
static pthread_cond_t _bug20_cond1;
static volatile __attribute__((used)) int* volatile _bug20_ptr;
static volatile __attribute__((used)) int _bug20_val;
static volatile __attribute__((used)) int _bug20_pred;
static void _bug20_pattern_clean(void)
{
    pthread_mutex_init(&_bug20_mutex1, NULL);
    pthread_cond_init(&_bug20_cond1, NULL);
    _bug20_ptr = NULL;
    _bug20_val = 0;
    _bug20_pred = 0;
}
static __attribute__((optimize(0))) int _bug20_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug20_pred = 0;
        pthread_cond_signal(&_bug20_cond1);
        pthread_mutex_lock(&_bug20_mutex1);
        _bug20_ptr = &_bug20_val;
        _bug20_pred = 1;
        pthread_mutex_unlock(&_bug20_mutex1);
        pthread_cond_signal(&_bug20_cond1);
    }
    else {
        pthread_mutex_lock(&_bug20_mutex1);
        if (!_bug20_pred)
            pthread_cond_wait(&_bug20_cond1, &_bug20_mutex1);
        if (_bug20_ptr == NULL)
            trigger = 1;
        _bug20_ptr = NULL;
        _bug20_pred = 0;
        pthread_mutex_unlock(&_bug20_mutex1);
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
    static const uint32_t indices[] = {0x7e9c7};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182d1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug20_input_helper.interleave_num = 0;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xa1182d1e;
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
static pthread_cond_t _bug21_cond1;
static volatile __attribute__((used)) int* volatile _bug21_ptr;
static volatile __attribute__((used)) int _bug21_val;
static volatile __attribute__((used)) int _bug21_pred;
static void _bug21_pattern_clean(void)
{
    pthread_mutex_init(&_bug21_mutex1, NULL);
    pthread_cond_init(&_bug21_cond1, NULL);
    _bug21_ptr = NULL;
    _bug21_val = 0;
    _bug21_pred = 0;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug21_pred = 0;
        pthread_cond_signal(&_bug21_cond1);
        pthread_mutex_lock(&_bug21_mutex1);
        _bug21_ptr = &_bug21_val;
        _bug21_pred = 1;
        pthread_mutex_unlock(&_bug21_mutex1);
        pthread_cond_signal(&_bug21_cond1);
    }
    else {
        pthread_mutex_lock(&_bug21_mutex1);
        if (!_bug21_pred)
            pthread_cond_wait(&_bug21_cond1, &_bug21_mutex1);
        if (_bug21_ptr == NULL)
            trigger = 1;
        _bug21_ptr = NULL;
        _bug21_pred = 0;
        pthread_mutex_unlock(&_bug21_mutex1);
    }
    return trigger;
}

static void _bug21_pattern_clean(void);
static int _bug21_pattern(int part);
static racebench_bug_helper _bug21_bug_helper;
static racebench_input_helper _bug21_input_helper;
__attribute__((constructor))
static void _bug21_input_helper_init(void)
{
    static const uint32_t indices[] = {0x11f0dd};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x97961c51};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cfe};
    static racebench_before_helper before_helpers[2] = {};
    _bug21_input_helper.interleave_num = 2;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0x38ae494f;
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
static volatile __attribute__((used)) int _bug22_arr[3];
static volatile __attribute__((used)) int _bug22_val;
static volatile __attribute__((used)) int _bug22_pred;
static void _bug22_pattern_clean(void)
{
    _bug22_arr[0] = 0, _bug22_arr[1] = 2, _bug22_arr[2] = 1;
    _bug22_val = 0;
    _bug22_pred = 0;
}
static __attribute__((optimize(0))) int _bug22_pattern(int part)
{
    int trigger = 0;
    _bug22_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug22_val += _bug22_arr[i];
        }
        __sync_synchronize();
        while (!_bug22_pred)
            pthread_yield();
        trigger = _bug22_val != 3;
        _bug22_pred = 0;
        _bug22_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug22_val += _bug22_arr[i];
        }
        __sync_synchronize();
        _bug22_pred = 1;
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
    static const uint32_t indices[] = {0x3246e0};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa117ee7e};
    static racebench_before_helper before_helpers[1] = {};
    _bug22_input_helper.interleave_num = 1;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0xa117ee7e;
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
static volatile __attribute__((used)) int _bug23_arr[3];
static volatile __attribute__((used)) int _bug23_val;
static volatile __attribute__((used)) int _bug23_pred;
static void _bug23_pattern_clean(void)
{
    _bug23_arr[0] = 0, _bug23_arr[1] = 2, _bug23_arr[2] = 1;
    _bug23_val = 0;
    _bug23_pred = 0;
}
static __attribute__((optimize(0))) int _bug23_pattern(int part)
{
    int trigger = 0;
    _bug23_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug23_val += _bug23_arr[i];
        }
        __sync_synchronize();
        while (!_bug23_pred)
            pthread_yield();
        trigger = _bug23_val != 3;
        _bug23_pred = 0;
        _bug23_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug23_val += _bug23_arr[i];
        }
        __sync_synchronize();
        _bug23_pred = 1;
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
    static const uint32_t indices[] = {0x187295};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8414b829};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182eee};
    static racebench_before_helper before_helpers[2] = {};
    _bug23_input_helper.interleave_num = 2;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0x252ce717;
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
static volatile __attribute__((used)) int _bug24_val;
static void _bug24_pattern_clean(void)
{
    pthread_mutex_init(&_bug24_mutex1, NULL);
    _bug24_val = 0;
}
static __attribute__((optimize(0))) int _bug24_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug24_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug24_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug24_mutex1);
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

static void _bug24_pattern_clean(void);
static int _bug24_pattern(int part);
static racebench_bug_helper _bug24_bug_helper;
static racebench_input_helper _bug24_input_helper;
__attribute__((constructor))
static void _bug24_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2b9502};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x16c19b1c, 0x1a33c9ae};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182fee};
    static racebench_before_helper before_helpers[3] = {};
    _bug24_input_helper.interleave_num = 3;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0xd20d94b8;
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
static volatile __attribute__((used)) int _bug25_val;
static void _bug25_pattern_clean(void)
{
    pthread_mutex_init(&_bug25_mutex1, NULL);
    _bug25_val = 0;
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug25_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug25_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug25_mutex1);
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

static void _bug25_pattern_clean(void);
static int _bug25_pattern(int part);
static racebench_bug_helper _bug25_bug_helper;
static racebench_input_helper _bug25_input_helper;
__attribute__((constructor))
static void _bug25_input_helper_init(void)
{
    static const uint32_t indices[] = {0x37eab8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x439f11a4, 0x6a5d2297};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa111a6de};
    static racebench_before_helper before_helpers[3] = {};
    _bug25_input_helper.interleave_num = 3;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0x4f0ddb19;
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
static pthread_mutex_t _bug26_mutex1;
static volatile __attribute__((used)) int _bug26_first_part;
static void _bug26_pattern_clean(void)
{
    pthread_mutex_init(&_bug26_mutex1, NULL);
    _bug26_first_part = -1;
}
static __attribute__((optimize(0))) int _bug26_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug26_mutex1);
        if (_bug26_first_part == -1)
            _bug26_first_part = part;
        else
            _bug26_first_part = -1;
        pthread_mutex_unlock(&_bug26_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug26_mutex1);
        if (_bug26_first_part == -1) {
            _bug26_first_part = part;
            trigger = 1;
        }
        else
            _bug26_first_part = -1;
        pthread_mutex_unlock(&_bug26_mutex1);
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
    static const uint32_t indices[] = {0x255cf8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x677c9cc1};
    static racebench_before_helper before_helpers[1] = {};
    _bug26_input_helper.interleave_num = 1;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0x677c9cc1;
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
static pthread_mutex_t _bug27_mutex1;
static volatile __attribute__((used)) int _bug27_first_part;
static volatile __attribute__((used)) int _bug27_released;
static void _bug27_pattern_clean(void)
{
    pthread_mutex_init(&_bug27_mutex1, NULL);
    _bug27_first_part = -1;
    _bug27_released = -1;
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug27_mutex1) == 0)
                break;
            if (_bug27_released == 0) {
                _bug27_first_part = -1;
                _bug27_released = -1;
                return 1;
            }
        }
        if (_bug27_first_part == -1)
            _bug27_first_part = part;
        else {
            _bug27_first_part = -1;
            while (_bug27_released == -1) {}
            _bug27_released = -1;
        }
        pthread_mutex_unlock(&_bug27_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug27_mutex1);
        if (_bug27_first_part == -1) {
            _bug27_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug27_mutex1);
                _bug27_released = 1;
            }
            else {
                _bug27_released = 0;
                while (_bug27_released != -1) {}
                pthread_mutex_unlock(&_bug27_mutex1);
                return 1;
            }
        }
        else {
            _bug27_first_part = -1;
            pthread_mutex_unlock(&_bug27_mutex1);
        }
    }
    return 0;
}

static void _bug27_pattern_clean(void);
static int _bug27_pattern(int part);
static racebench_bug_helper _bug27_bug_helper;
static racebench_input_helper _bug27_input_helper;
__attribute__((constructor))
static void _bug27_input_helper_init(void)
{
    static const uint32_t indices[] = {0x184530};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd6983027, 0x348e2a3a};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x640c3c69};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0x6f3296ca;
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
static volatile __attribute__((used)) int* volatile _bug28_ptr;
static volatile __attribute__((used)) int _bug28_val;
static void _bug28_pattern_clean(void)
{
    pthread_mutex_init(&_bug28_mutex1, NULL);
    _bug28_ptr = NULL;
    _bug28_val = 0;
}
static __attribute__((optimize(0))) int _bug28_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug28_mutex1);
        _bug28_ptr = &_bug28_val;
        pthread_mutex_unlock(&_bug28_mutex1);
        pthread_mutex_lock(&_bug28_mutex1);
        if (_bug28_ptr == NULL)
            trigger = 1;
        _bug28_ptr = NULL;
        pthread_mutex_unlock(&_bug28_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug28_mutex1);
        _bug28_ptr = &_bug28_val;
        if (_bug28_ptr == NULL)
            trigger = 1;
        _bug28_ptr = NULL;
        pthread_mutex_unlock(&_bug28_mutex1);
    }
    return trigger;
}

static void _bug28_pattern_clean(void);
static int _bug28_pattern(int part);
static racebench_bug_helper _bug28_bug_helper;
static racebench_input_helper _bug28_input_helper;
__attribute__((constructor))
static void _bug28_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5aeb9};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[0] = {};
    _bug28_input_helper.interleave_num = 0;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa1182c8e;
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
static volatile __attribute__((used)) int _bug29_arr[3];
static volatile __attribute__((used)) int _bug29_val;
static volatile __attribute__((used)) int _bug29_pred;
static void _bug29_pattern_clean(void)
{
    _bug29_arr[0] = 0, _bug29_arr[1] = 2, _bug29_arr[2] = 1;
    _bug29_val = 0;
    _bug29_pred = 0;
}
static __attribute__((optimize(0))) int _bug29_pattern(int part)
{
    int trigger = 0;
    _bug29_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug29_val += _bug29_arr[i];
        }
        __sync_synchronize();
        while (!_bug29_pred)
            pthread_yield();
        trigger = _bug29_val != 3;
        _bug29_pred = 0;
        _bug29_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug29_val += _bug29_arr[i];
        }
        __sync_synchronize();
        _bug29_pred = 1;
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
    static const uint32_t indices[] = {0x287636};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x5e0a719, 0xcbe73aff};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x952b5fcf};
    static racebench_before_helper before_helpers[3] = {};
    _bug29_input_helper.interleave_num = 3;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x66f341e7;
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
    static const uint32_t indices[] = {0x7e33f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x30942764, 0xb0f14112};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1158eae};
    static racebench_before_helper before_helpers[3] = {};
    _bug30_input_helper.interleave_num = 3;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0x829af724;
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
static volatile __attribute__((used)) int _bug31_ok;
static volatile __attribute__((used)) int* volatile _bug31_ptr;
static volatile __attribute__((used)) int _bug31_val;
static void _bug31_pattern_clean(void)
{
    _bug31_ok = 0;
    _bug31_ptr = NULL;
    _bug31_val = 0;
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug31_ok = 1;
        __sync_synchronize();
        _bug31_ptr = &_bug31_val;
    }
    else {
        while (!_bug31_ok)
            pthread_yield();
        if (_bug31_ptr == NULL)
            trigger = 1;
        _bug31_ptr = NULL;
        _bug31_ok = 0;
    }
    return trigger;
}

static void _bug31_pattern_clean(void);
static int _bug31_pattern(int part);
static racebench_bug_helper _bug31_bug_helper;
static racebench_input_helper _bug31_input_helper;
__attribute__((constructor))
static void _bug31_input_helper_init(void)
{
    static const uint32_t indices[] = {0x15cc03};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug31_input_helper.interleave_num = 1;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0xa1182f1e;
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
static pthread_mutex_t _bug32_mutex2;
static void _bug32_pattern_clean(void)
{
    pthread_mutex_init(&_bug32_mutex1, NULL);
    pthread_mutex_init(&_bug32_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug32_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug32_mutex1);
        if (pthread_mutex_trylock(&_bug32_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug32_mutex2);
        pthread_mutex_unlock(&_bug32_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug32_mutex2);
        if (pthread_mutex_trylock(&_bug32_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug32_mutex1);
        pthread_mutex_unlock(&_bug32_mutex2);
    }
    return trigger;
}

static void _bug32_pattern_clean(void);
static int _bug32_pattern(int part);
static racebench_bug_helper _bug32_bug_helper;
static racebench_input_helper _bug32_input_helper;
__attribute__((constructor))
static void _bug32_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5168e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa118266e};
    static racebench_before_helper before_helpers[1] = {};
    _bug32_input_helper.interleave_num = 1;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0xa118266e;
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
    static const uint32_t indices[] = {0x1c4c67};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112fcae};
    static racebench_before_helper before_helpers[0] = {};
    _bug33_input_helper.interleave_num = 0;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0xa112fcae;
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
static pthread_mutex_t _bug34_mutex1;
static volatile __attribute__((used)) int _bug34_first_part;
static void _bug34_pattern_clean(void)
{
    pthread_mutex_init(&_bug34_mutex1, NULL);
    _bug34_first_part = -1;
}
static __attribute__((optimize(0))) int _bug34_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug34_mutex1);
        if (_bug34_first_part == -1)
            _bug34_first_part = part;
        else
            _bug34_first_part = -1;
        pthread_mutex_unlock(&_bug34_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug34_mutex1);
        if (_bug34_first_part == -1) {
            _bug34_first_part = part;
            trigger = 1;
        }
        else
            _bug34_first_part = -1;
        pthread_mutex_unlock(&_bug34_mutex1);
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
    static const uint32_t indices[] = {0xdbc2};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xe6a34a8b};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xc6850ecc};
    static racebench_before_helper before_helpers[2] = {};
    _bug34_input_helper.interleave_num = 2;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0xad285957;
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
static pthread_mutex_t _bug35_mutex1;
static volatile __attribute__((used)) int _bug35_first_part;
static volatile __attribute__((used)) int _bug35_released;
static void _bug35_pattern_clean(void)
{
    pthread_mutex_init(&_bug35_mutex1, NULL);
    _bug35_first_part = -1;
    _bug35_released = -1;
}
static __attribute__((optimize(0))) int _bug35_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug35_mutex1) == 0)
                break;
            if (_bug35_released == 0) {
                _bug35_first_part = -1;
                _bug35_released = -1;
                return 1;
            }
        }
        if (_bug35_first_part == -1)
            _bug35_first_part = part;
        else {
            _bug35_first_part = -1;
            while (_bug35_released == -1) {}
            _bug35_released = -1;
        }
        pthread_mutex_unlock(&_bug35_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug35_mutex1);
        if (_bug35_first_part == -1) {
            _bug35_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug35_mutex1);
                _bug35_released = 1;
            }
            else {
                _bug35_released = 0;
                while (_bug35_released != -1) {}
                pthread_mutex_unlock(&_bug35_mutex1);
                return 1;
            }
        }
        else {
            _bug35_first_part = -1;
            pthread_mutex_unlock(&_bug35_mutex1);
        }
    }
    return 0;
}

static void _bug35_pattern_clean(void);
static int _bug35_pattern(int part);
static racebench_bug_helper _bug35_bug_helper;
static racebench_input_helper _bug35_input_helper;
__attribute__((constructor))
static void _bug35_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1eabdb};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182dde};
    static racebench_before_helper before_helpers[0] = {};
    _bug35_input_helper.interleave_num = 0;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xa1182dde;
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
static volatile __attribute__((used)) int* volatile _bug36_ptr;
static volatile __attribute__((used)) int _bug36_val;
static void _bug36_pattern_clean(void)
{
    pthread_mutex_init(&_bug36_mutex1, NULL);
    _bug36_ptr = NULL;
    _bug36_val = 0;
}
static __attribute__((optimize(0))) int _bug36_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug36_mutex1);
        _bug36_ptr = &_bug36_val;
        pthread_mutex_unlock(&_bug36_mutex1);
        pthread_mutex_lock(&_bug36_mutex1);
        if (_bug36_ptr == NULL)
            trigger = 1;
        _bug36_ptr = NULL;
        pthread_mutex_unlock(&_bug36_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug36_mutex1);
        _bug36_ptr = &_bug36_val;
        if (_bug36_ptr == NULL)
            trigger = 1;
        _bug36_ptr = NULL;
        pthread_mutex_unlock(&_bug36_mutex1);
    }
    return trigger;
}

static void _bug36_pattern_clean(void);
static int _bug36_pattern(int part);
static racebench_bug_helper _bug36_bug_helper;
static racebench_input_helper _bug36_input_helper;
__attribute__((constructor))
static void _bug36_input_helper_init(void)
{
    static const uint32_t indices[] = {0x13ee7};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x1b34ae82, 0xa1b6843c};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11825ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug36_input_helper.interleave_num = 3;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0x5e03586c;
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
static pthread_mutex_t _bug37_mutex1;
static volatile __attribute__((used)) int* volatile _bug37_ptr;
static volatile __attribute__((used)) int _bug37_val;
static void _bug37_pattern_clean(void)
{
    pthread_mutex_init(&_bug37_mutex1, NULL);
    _bug37_ptr = NULL;
    _bug37_val = 0;
}
static __attribute__((optimize(0))) int _bug37_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug37_mutex1);
        _bug37_ptr = &_bug37_val;
        pthread_mutex_unlock(&_bug37_mutex1);
        pthread_mutex_lock(&_bug37_mutex1);
        if (_bug37_ptr == NULL)
            trigger = 1;
        _bug37_ptr = NULL;
        pthread_mutex_unlock(&_bug37_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug37_mutex1);
        _bug37_ptr = &_bug37_val;
        if (_bug37_ptr == NULL)
            trigger = 1;
        _bug37_ptr = NULL;
        pthread_mutex_unlock(&_bug37_mutex1);
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
    static const uint32_t indices[] = {0x3422f6};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa359626d, 0x2efa6097};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa473cc6f};
    static racebench_before_helper before_helpers[3] = {};
    _bug37_input_helper.interleave_num = 3;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0x76c78f73;
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
static pthread_mutex_t _bug38_mutex1;
static volatile __attribute__((used)) int* volatile _bug38_ptr;
static volatile __attribute__((used)) int _bug38_val;
static void _bug38_pattern_clean(void)
{
    pthread_mutex_init(&_bug38_mutex1, NULL);
    _bug38_ptr = NULL;
    _bug38_val = 0;
}
static __attribute__((optimize(0))) int _bug38_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug38_mutex1);
        _bug38_ptr = &_bug38_val;
        pthread_mutex_unlock(&_bug38_mutex1);
        pthread_mutex_lock(&_bug38_mutex1);
        if (_bug38_ptr == NULL)
            trigger = 1;
        _bug38_ptr = NULL;
        pthread_mutex_unlock(&_bug38_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug38_mutex1);
        _bug38_ptr = &_bug38_val;
        if (_bug38_ptr == NULL)
            trigger = 1;
        _bug38_ptr = NULL;
        pthread_mutex_unlock(&_bug38_mutex1);
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
    static const uint32_t indices[] = {0x250bd3};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa118231e};
    static racebench_before_helper before_helpers[1] = {};
    _bug38_input_helper.interleave_num = 1;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xa118231e;
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
static volatile __attribute__((used)) int _bug39_arr[3];
static volatile __attribute__((used)) int _bug39_val;
static volatile __attribute__((used)) int _bug39_pred;
static void _bug39_pattern_clean(void)
{
    _bug39_arr[0] = 0, _bug39_arr[1] = 2, _bug39_arr[2] = 1;
    _bug39_val = 0;
    _bug39_pred = 0;
}
static __attribute__((optimize(0))) int _bug39_pattern(int part)
{
    int trigger = 0;
    _bug39_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug39_val += _bug39_arr[i];
        }
        __sync_synchronize();
        while (!_bug39_pred)
            pthread_yield();
        trigger = _bug39_val != 3;
        _bug39_pred = 0;
        _bug39_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug39_val += _bug39_arr[i];
        }
        __sync_synchronize();
        _bug39_pred = 1;
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
    static const uint32_t indices[] = {0x37799a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xe4c50f7c};
    static racebench_before_helper before_helpers[0] = {};
    _bug39_input_helper.interleave_num = 0;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0xe4c50f7c;
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
static volatile __attribute__((used)) int _bug40_val;
static void _bug40_pattern_clean(void)
{
    pthread_mutex_init(&_bug40_mutex1, NULL);
    _bug40_val = 0;
}
static __attribute__((optimize(0))) int _bug40_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug40_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug40_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug40_mutex1);
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

static void _bug40_pattern_clean(void);
static int _bug40_pattern(int part);
static racebench_bug_helper _bug40_bug_helper;
static racebench_input_helper _bug40_input_helper;
__attribute__((constructor))
static void _bug40_input_helper_init(void)
{
    static const uint32_t indices[] = {0x30126a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1165ade};
    static racebench_before_helper before_helpers[0] = {};
    _bug40_input_helper.interleave_num = 0;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xa1165ade;
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
static pthread_mutex_t _bug41_mutex1;
static volatile __attribute__((used)) int* volatile _bug41_ptr;
static volatile __attribute__((used)) int _bug41_val;
static void _bug41_pattern_clean(void)
{
    pthread_mutex_init(&_bug41_mutex1, NULL);
    _bug41_ptr = NULL;
    _bug41_val = 0;
}
static __attribute__((optimize(0))) int _bug41_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug41_mutex1);
        _bug41_ptr = &_bug41_val;
        pthread_mutex_unlock(&_bug41_mutex1);
        pthread_mutex_lock(&_bug41_mutex1);
        if (_bug41_ptr == NULL)
            trigger = 1;
        _bug41_ptr = NULL;
        pthread_mutex_unlock(&_bug41_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug41_mutex1);
        _bug41_ptr = &_bug41_val;
        if (_bug41_ptr == NULL)
            trigger = 1;
        _bug41_ptr = NULL;
        pthread_mutex_unlock(&_bug41_mutex1);
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
    static const uint32_t indices[] = {0x285c30};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x26c60d8c};
    static racebench_before_helper before_helpers[0] = {};
    _bug41_input_helper.interleave_num = 0;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0x26c60d8c;
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
static volatile __attribute__((used)) int _bug42_ok;
static volatile __attribute__((used)) int* volatile _bug42_ptr;
static volatile __attribute__((used)) int _bug42_val;
static void _bug42_pattern_clean(void)
{
    _bug42_ok = 0;
    _bug42_ptr = NULL;
    _bug42_val = 0;
}
static __attribute__((optimize(0))) int _bug42_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug42_ok = 1;
        __sync_synchronize();
        _bug42_ptr = &_bug42_val;
    }
    else {
        while (!_bug42_ok)
            pthread_yield();
        if (_bug42_ptr == NULL)
            trigger = 1;
        _bug42_ptr = NULL;
        _bug42_ok = 0;
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
    static const uint32_t indices[] = {0x16853a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c2e};
    static racebench_before_helper before_helpers[1] = {};
    _bug42_input_helper.interleave_num = 1;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0xa1182c2e;
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
    static const uint32_t indices[] = {0x26d2a6};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x1835e89f};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug43_input_helper.interleave_num = 2;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0xb94e155d;
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
static volatile __attribute__((used)) int _bug44_first_part;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    _bug44_first_part = -1;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug44_mutex1);
        if (_bug44_first_part == -1)
            _bug44_first_part = part;
        else
            _bug44_first_part = -1;
        pthread_mutex_unlock(&_bug44_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug44_mutex1);
        if (_bug44_first_part == -1) {
            _bug44_first_part = part;
            trigger = 1;
        }
        else
            _bug44_first_part = -1;
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
    static const uint32_t indices[] = {0x13984b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112cdae};
    static racebench_before_helper before_helpers[0] = {};
    _bug44_input_helper.interleave_num = 0;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xa112cdae;
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
static pthread_cond_t _bug45_cond1;
static volatile __attribute__((used)) int _bug45_pred;
static volatile __attribute__((used)) int _bug45_sent;
static void _bug45_pattern_clean(void)
{
    pthread_mutex_init(&_bug45_mutex1, NULL);
    pthread_cond_init(&_bug45_cond1, NULL);
    _bug45_pred = 0;
    _bug45_sent = 0;
}
static __attribute__((optimize(0))) int _bug45_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug45_mutex1);
        _bug45_pred = 1;
        pthread_cond_signal(&_bug45_cond1);
        _bug45_sent = 1;
        pthread_mutex_unlock(&_bug45_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug45_mutex1);
        if (_bug45_sent) {
            trigger = 1;
        }
        else {
            while (_bug45_pred == 0)
                pthread_cond_wait(&_bug45_cond1, &_bug45_mutex1);
        }
        _bug45_pred = 0;
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
    static const uint32_t indices[] = {0x1d9f79};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x5117ee69, 0x3e4e7667};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug45_input_helper.interleave_num = 3;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0x307e934e;
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
    static const uint32_t indices[] = {0x5cfec};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2994fac9};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1153dde};
    static racebench_before_helper before_helpers[2] = {};
    _bug46_input_helper.interleave_num = 2;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0xcaaa38a7;
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
static volatile __attribute__((used)) int _bug47_arr[3];
static volatile __attribute__((used)) int _bug47_val;
static volatile __attribute__((used)) int _bug47_pred;
static void _bug47_pattern_clean(void)
{
    _bug47_arr[0] = 0, _bug47_arr[1] = 2, _bug47_arr[2] = 1;
    _bug47_val = 0;
    _bug47_pred = 0;
}
static __attribute__((optimize(0))) int _bug47_pattern(int part)
{
    int trigger = 0;
    _bug47_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug47_val += _bug47_arr[i];
        }
        __sync_synchronize();
        while (!_bug47_pred)
            pthread_yield();
        trigger = _bug47_val != 3;
        _bug47_pred = 0;
        _bug47_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug47_val += _bug47_arr[i];
        }
        __sync_synchronize();
        _bug47_pred = 1;
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
    static const uint32_t indices[] = {0x1e83e6};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115adde};
    static racebench_before_helper before_helpers[0] = {};
    _bug47_input_helper.interleave_num = 0;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0xa115adde;
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
static pthread_mutex_t _bug48_mutex1;
static volatile __attribute__((used)) int _bug48_first_part;
static volatile __attribute__((used)) int _bug48_released;
static void _bug48_pattern_clean(void)
{
    pthread_mutex_init(&_bug48_mutex1, NULL);
    _bug48_first_part = -1;
    _bug48_released = -1;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug48_mutex1) == 0)
                break;
            if (_bug48_released == 0) {
                _bug48_first_part = -1;
                _bug48_released = -1;
                return 1;
            }
        }
        if (_bug48_first_part == -1)
            _bug48_first_part = part;
        else {
            _bug48_first_part = -1;
            while (_bug48_released == -1) {}
            _bug48_released = -1;
        }
        pthread_mutex_unlock(&_bug48_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug48_mutex1);
        if (_bug48_first_part == -1) {
            _bug48_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug48_mutex1);
                _bug48_released = 1;
            }
            else {
                _bug48_released = 0;
                while (_bug48_released != -1) {}
                pthread_mutex_unlock(&_bug48_mutex1);
                return 1;
            }
        }
        else {
            _bug48_first_part = -1;
            pthread_mutex_unlock(&_bug48_mutex1);
        }
    }
    return 0;
}

static void _bug48_pattern_clean(void);
static int _bug48_pattern(int part);
static racebench_bug_helper _bug48_bug_helper;
static racebench_input_helper _bug48_input_helper;
__attribute__((constructor))
static void _bug48_input_helper_init(void)
{
    static const uint32_t indices[] = {0xffc76};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x4020f57};
    static racebench_before_helper before_helpers[1] = {};
    _bug48_input_helper.interleave_num = 1;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0x4020f57;
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
static volatile __attribute__((used)) int _bug49_ok;
static volatile __attribute__((used)) int* volatile _bug49_ptr;
static volatile __attribute__((used)) int _bug49_val;
static void _bug49_pattern_clean(void)
{
    _bug49_ok = 0;
    _bug49_ptr = NULL;
    _bug49_val = 0;
}
static __attribute__((optimize(0))) int _bug49_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug49_ok = 1;
        __sync_synchronize();
        _bug49_ptr = &_bug49_val;
    }
    else {
        while (!_bug49_ok)
            pthread_yield();
        if (_bug49_ptr == NULL)
            trigger = 1;
        _bug49_ptr = NULL;
        _bug49_ok = 0;
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
    static const uint32_t indices[] = {0x2abb29};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf8e4251e, 0x62e63bd8};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1166c1e};
    static racebench_before_helper before_helpers[3] = {};
    _bug49_input_helper.interleave_num = 3;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xfce0cd14;
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
    static const uint32_t indices[] = {0x11a88e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182aee};
    static racebench_before_helper before_helpers[1] = {};
    _bug50_input_helper.interleave_num = 1;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0xa1182aee;
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
static pthread_mutex_t _bug51_mutex2;
static void _bug51_pattern_clean(void)
{
    pthread_mutex_init(&_bug51_mutex1, NULL);
    pthread_mutex_init(&_bug51_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug51_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug51_mutex1);
        if (pthread_mutex_trylock(&_bug51_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug51_mutex2);
        pthread_mutex_unlock(&_bug51_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug51_mutex2);
        if (pthread_mutex_trylock(&_bug51_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug51_mutex1);
        pthread_mutex_unlock(&_bug51_mutex2);
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
    static const uint32_t indices[] = {0x1ac5df};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1143b8e};
    static racebench_before_helper before_helpers[0] = {};
    _bug51_input_helper.interleave_num = 0;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0xa1143b8e;
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
static pthread_cond_t _bug52_cond1;
static volatile __attribute__((used)) int* volatile _bug52_ptr;
static volatile __attribute__((used)) int _bug52_val;
static volatile __attribute__((used)) int _bug52_pred;
static void _bug52_pattern_clean(void)
{
    pthread_mutex_init(&_bug52_mutex1, NULL);
    pthread_cond_init(&_bug52_cond1, NULL);
    _bug52_ptr = NULL;
    _bug52_val = 0;
    _bug52_pred = 0;
}
static __attribute__((optimize(0))) int _bug52_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug52_pred = 0;
        pthread_cond_signal(&_bug52_cond1);
        pthread_mutex_lock(&_bug52_mutex1);
        _bug52_ptr = &_bug52_val;
        _bug52_pred = 1;
        pthread_mutex_unlock(&_bug52_mutex1);
        pthread_cond_signal(&_bug52_cond1);
    }
    else {
        pthread_mutex_lock(&_bug52_mutex1);
        if (!_bug52_pred)
            pthread_cond_wait(&_bug52_cond1, &_bug52_mutex1);
        if (_bug52_ptr == NULL)
            trigger = 1;
        _bug52_ptr = NULL;
        _bug52_pred = 0;
        pthread_mutex_unlock(&_bug52_mutex1);
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
    static const uint32_t indices[] = {0x57540};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa117dbde};
    static racebench_before_helper before_helpers[0] = {};
    _bug52_input_helper.interleave_num = 0;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0xa117dbde;
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
static pthread_mutex_t _bug53_mutex1;
static volatile __attribute__((used)) int _bug53_first_part;
static void _bug53_pattern_clean(void)
{
    pthread_mutex_init(&_bug53_mutex1, NULL);
    _bug53_first_part = -1;
}
static __attribute__((optimize(0))) int _bug53_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug53_mutex1);
        if (_bug53_first_part == -1)
            _bug53_first_part = part;
        else
            _bug53_first_part = -1;
        pthread_mutex_unlock(&_bug53_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug53_mutex1);
        if (_bug53_first_part == -1) {
            _bug53_first_part = part;
            trigger = 1;
        }
        else
            _bug53_first_part = -1;
        pthread_mutex_unlock(&_bug53_mutex1);
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
    static const uint32_t indices[] = {0x10e688};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x622aa316, 0x341e67f1};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xd665bcbf};
    static racebench_before_helper before_helpers[3] = {};
    _bug53_input_helper.interleave_num = 3;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0x6caec7c6;
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
static volatile __attribute__((used)) int _bug54_first_part;
static void _bug54_pattern_clean(void)
{
    pthread_mutex_init(&_bug54_mutex1, NULL);
    _bug54_first_part = -1;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug54_mutex1);
        if (_bug54_first_part == -1)
            _bug54_first_part = part;
        else
            _bug54_first_part = -1;
        pthread_mutex_unlock(&_bug54_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug54_mutex1);
        if (_bug54_first_part == -1) {
            _bug54_first_part = part;
            trigger = 1;
        }
        else
            _bug54_first_part = -1;
        pthread_mutex_unlock(&_bug54_mutex1);
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
    static const uint32_t indices[] = {0xbfd21};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x9562b105, 0x5ac3e2b};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11b6bae};
    static racebench_before_helper before_helpers[3] = {};
    _bug54_input_helper.interleave_num = 3;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0x3c2a5ade;
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
static volatile __attribute__((used)) int _bug55_first_part;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    _bug55_first_part = -1;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug55_mutex1);
        if (_bug55_first_part == -1)
            _bug55_first_part = part;
        else
            _bug55_first_part = -1;
        pthread_mutex_unlock(&_bug55_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug55_mutex1);
        if (_bug55_first_part == -1) {
            _bug55_first_part = part;
            trigger = 1;
        }
        else
            _bug55_first_part = -1;
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
    static const uint32_t indices[] = {0x1c8143};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x8d46a3b7, 0xfd2a409a};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x25329bed};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0xafa3803e;
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
static volatile __attribute__((used)) int _bug56_ok;
static volatile __attribute__((used)) int* volatile _bug56_ptr;
static volatile __attribute__((used)) int _bug56_val;
static void _bug56_pattern_clean(void)
{
    _bug56_ok = 0;
    _bug56_ptr = NULL;
    _bug56_val = 0;
}
static __attribute__((optimize(0))) int _bug56_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug56_ok = 1;
        __sync_synchronize();
        _bug56_ptr = &_bug56_val;
    }
    else {
        while (!_bug56_ok)
            pthread_yield();
        if (_bug56_ptr == NULL)
            trigger = 1;
        _bug56_ptr = NULL;
        _bug56_ok = 0;
    }
    return trigger;
}

static void _bug56_pattern_clean(void);
static int _bug56_pattern(int part);
static racebench_bug_helper _bug56_bug_helper;
static racebench_input_helper _bug56_input_helper;
__attribute__((constructor))
static void _bug56_input_helper_init(void)
{
    static const uint32_t indices[] = {0x7978f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x38056f10};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11830fe};
    static racebench_before_helper before_helpers[2] = {};
    _bug56_input_helper.interleave_num = 2;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0xd91da00e;
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
static volatile __attribute__((used)) int _bug57_val;
static void _bug57_pattern_clean(void)
{
    pthread_mutex_init(&_bug57_mutex1, NULL);
    _bug57_val = 0;
}
static __attribute__((optimize(0))) int _bug57_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug57_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug57_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug57_mutex1);
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

static void _bug57_pattern_clean(void);
static int _bug57_pattern(int part);
static racebench_bug_helper _bug57_bug_helper;
static racebench_input_helper _bug57_input_helper;
__attribute__((constructor))
static void _bug57_input_helper_init(void)
{
    static const uint32_t indices[] = {0x29fb61};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa116dbfe};
    static racebench_before_helper before_helpers[0] = {};
    _bug57_input_helper.interleave_num = 0;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0xa116dbfe;
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
static volatile __attribute__((used)) int _bug58_val;
static void _bug58_pattern_clean(void)
{
    pthread_mutex_init(&_bug58_mutex1, NULL);
    _bug58_val = 0;
}
static __attribute__((optimize(0))) int _bug58_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug58_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug58_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug58_mutex1);
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

static void _bug58_pattern_clean(void);
static int _bug58_pattern(int part);
static racebench_bug_helper _bug58_bug_helper;
static racebench_input_helper _bug58_input_helper;
__attribute__((constructor))
static void _bug58_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2985eb};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf47b7cb9};
    static racebench_before_helper before_helpers[0] = {};
    _bug58_input_helper.interleave_num = 0;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0xf47b7cb9;
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
static volatile __attribute__((used)) int _bug59_first_part;
static volatile __attribute__((used)) int _bug59_released;
static void _bug59_pattern_clean(void)
{
    pthread_mutex_init(&_bug59_mutex1, NULL);
    _bug59_first_part = -1;
    _bug59_released = -1;
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug59_mutex1) == 0)
                break;
            if (_bug59_released == 0) {
                _bug59_first_part = -1;
                _bug59_released = -1;
                return 1;
            }
        }
        if (_bug59_first_part == -1)
            _bug59_first_part = part;
        else {
            _bug59_first_part = -1;
            while (_bug59_released == -1) {}
            _bug59_released = -1;
        }
        pthread_mutex_unlock(&_bug59_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug59_mutex1);
        if (_bug59_first_part == -1) {
            _bug59_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug59_mutex1);
                _bug59_released = 1;
            }
            else {
                _bug59_released = 0;
                while (_bug59_released != -1) {}
                pthread_mutex_unlock(&_bug59_mutex1);
                return 1;
            }
        }
        else {
            _bug59_first_part = -1;
            pthread_mutex_unlock(&_bug59_mutex1);
        }
    }
    return 0;
}

static void _bug59_pattern_clean(void);
static int _bug59_pattern(int part);
static racebench_bug_helper _bug59_bug_helper;
static racebench_input_helper _bug59_input_helper;
__attribute__((constructor))
static void _bug59_input_helper_init(void)
{
    static const uint32_t indices[] = {0x3536e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf2aec59};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xb4f40010};
    static racebench_before_helper before_helpers[2] = {};
    _bug59_input_helper.interleave_num = 2;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0xc41eec69;
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
static volatile __attribute__((used)) int _bug60_pred;
static volatile __attribute__((used)) int _bug60_sent;
static void _bug60_pattern_clean(void)
{
    pthread_mutex_init(&_bug60_mutex1, NULL);
    pthread_cond_init(&_bug60_cond1, NULL);
    _bug60_pred = 0;
    _bug60_sent = 0;
}
static __attribute__((optimize(0))) int _bug60_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug60_mutex1);
        _bug60_pred = 1;
        pthread_cond_signal(&_bug60_cond1);
        _bug60_sent = 1;
        pthread_mutex_unlock(&_bug60_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug60_mutex1);
        if (_bug60_sent) {
            trigger = 1;
        }
        else {
            while (_bug60_pred == 0)
                pthread_cond_wait(&_bug60_cond1, &_bug60_mutex1);
        }
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
    static const uint32_t indices[] = {0x252be2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x7e564cd5};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118313e};
    static racebench_before_helper before_helpers[2] = {};
    _bug60_input_helper.interleave_num = 2;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0x1f6e7e13;
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
static pthread_cond_t _bug61_cond1;
static volatile __attribute__((used)) int _bug61_pred;
static volatile __attribute__((used)) int _bug61_sent;
static void _bug61_pattern_clean(void)
{
    pthread_mutex_init(&_bug61_mutex1, NULL);
    pthread_cond_init(&_bug61_cond1, NULL);
    _bug61_pred = 0;
    _bug61_sent = 0;
}
static __attribute__((optimize(0))) int _bug61_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug61_mutex1);
        _bug61_pred = 1;
        pthread_cond_signal(&_bug61_cond1);
        _bug61_sent = 1;
        pthread_mutex_unlock(&_bug61_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug61_mutex1);
        if (_bug61_sent) {
            trigger = 1;
        }
        else {
            while (_bug61_pred == 0)
                pthread_cond_wait(&_bug61_cond1, &_bug61_mutex1);
        }
        _bug61_pred = 0;
        pthread_mutex_unlock(&_bug61_mutex1);
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
    static const uint32_t indices[] = {0x296490};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182bfe};
    static racebench_before_helper before_helpers[0] = {};
    _bug61_input_helper.interleave_num = 0;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0xa1182bfe;
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
static volatile __attribute__((used)) int _bug62_first_part;
static void _bug62_pattern_clean(void)
{
    pthread_mutex_init(&_bug62_mutex1, NULL);
    _bug62_first_part = -1;
}
static __attribute__((optimize(0))) int _bug62_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug62_mutex1);
        if (_bug62_first_part == -1)
            _bug62_first_part = part;
        else
            _bug62_first_part = -1;
        pthread_mutex_unlock(&_bug62_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug62_mutex1);
        if (_bug62_first_part == -1) {
            _bug62_first_part = part;
            trigger = 1;
        }
        else
            _bug62_first_part = -1;
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
    static const uint32_t indices[] = {0x80f76};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4ee9272b};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11538fe};
    static racebench_before_helper before_helpers[2] = {};
    _bug62_input_helper.interleave_num = 2;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0xeffe6029;
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
static volatile __attribute__((used)) int _bug63_val;
static void _bug63_pattern_clean(void)
{
    pthread_mutex_init(&_bug63_mutex1, NULL);
    _bug63_val = 0;
}
static __attribute__((optimize(0))) int _bug63_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug63_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug63_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug63_mutex1);
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

static void _bug63_pattern_clean(void);
static int _bug63_pattern(int part);
static racebench_bug_helper _bug63_bug_helper;
static racebench_input_helper _bug63_input_helper;
__attribute__((constructor))
static void _bug63_input_helper_init(void)
{
    static const uint32_t indices[] = {0x25674b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xfe7a1471};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182dfe};
    static racebench_before_helper before_helpers[2] = {};
    _bug63_input_helper.interleave_num = 2;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x9f92426f;
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
static volatile __attribute__((used)) int _bug64_arr[3];
static volatile __attribute__((used)) int _bug64_val;
static volatile __attribute__((used)) int _bug64_pred;
static void _bug64_pattern_clean(void)
{
    _bug64_arr[0] = 0, _bug64_arr[1] = 2, _bug64_arr[2] = 1;
    _bug64_val = 0;
    _bug64_pred = 0;
}
static __attribute__((optimize(0))) int _bug64_pattern(int part)
{
    int trigger = 0;
    _bug64_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug64_val += _bug64_arr[i];
        }
        __sync_synchronize();
        while (!_bug64_pred)
            pthread_yield();
        trigger = _bug64_val != 3;
        _bug64_pred = 0;
        _bug64_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug64_val += _bug64_arr[i];
        }
        __sync_synchronize();
        _bug64_pred = 1;
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
    static const uint32_t indices[] = {0x3467ba};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xb4224b94};
    static racebench_before_helper before_helpers[0] = {};
    _bug64_input_helper.interleave_num = 0;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0xb4224b94;
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
static pthread_cond_t _bug65_cond1;
static volatile __attribute__((used)) int _bug65_pred;
static volatile __attribute__((used)) int _bug65_sent;
static void _bug65_pattern_clean(void)
{
    pthread_mutex_init(&_bug65_mutex1, NULL);
    pthread_cond_init(&_bug65_cond1, NULL);
    _bug65_pred = 0;
    _bug65_sent = 0;
}
static __attribute__((optimize(0))) int _bug65_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug65_mutex1);
        _bug65_pred = 1;
        pthread_cond_signal(&_bug65_cond1);
        _bug65_sent = 1;
        pthread_mutex_unlock(&_bug65_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug65_mutex1);
        if (_bug65_sent) {
            trigger = 1;
        }
        else {
            while (_bug65_pred == 0)
                pthread_cond_wait(&_bug65_cond1, &_bug65_mutex1);
        }
        _bug65_pred = 0;
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
    static const uint32_t indices[] = {0x2acc7f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug65_input_helper.interleave_num = 1;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0xa1182c4e;
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
static volatile __attribute__((used)) int _bug66_arr[3];
static volatile __attribute__((used)) int _bug66_val;
static volatile __attribute__((used)) int _bug66_pred;
static void _bug66_pattern_clean(void)
{
    _bug66_arr[0] = 0, _bug66_arr[1] = 2, _bug66_arr[2] = 1;
    _bug66_val = 0;
    _bug66_pred = 0;
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    _bug66_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug66_val += _bug66_arr[i];
        }
        __sync_synchronize();
        while (!_bug66_pred)
            pthread_yield();
        trigger = _bug66_val != 3;
        _bug66_pred = 0;
        _bug66_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug66_val += _bug66_arr[i];
        }
        __sync_synchronize();
        _bug66_pred = 1;
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
    static const uint32_t indices[] = {0x1ff4b1};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xbd91cb7c, 0xa458aa7};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1170c4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug66_input_helper.interleave_num = 3;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0x68ee6271;
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
    static const uint32_t indices[] = {0x2a84d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x3e37a97c};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x23c118af};
    static racebench_before_helper before_helpers[2] = {};
    _bug67_input_helper.interleave_num = 2;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0x61f8c22b;
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
static volatile __attribute__((used)) int* volatile _bug68_ptr;
static volatile __attribute__((used)) int _bug68_val;
static volatile __attribute__((used)) int _bug68_pred;
static void _bug68_pattern_clean(void)
{
    pthread_mutex_init(&_bug68_mutex1, NULL);
    pthread_cond_init(&_bug68_cond1, NULL);
    _bug68_ptr = NULL;
    _bug68_val = 0;
    _bug68_pred = 0;
}
static __attribute__((optimize(0))) int _bug68_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug68_pred = 0;
        pthread_cond_signal(&_bug68_cond1);
        pthread_mutex_lock(&_bug68_mutex1);
        _bug68_ptr = &_bug68_val;
        _bug68_pred = 1;
        pthread_mutex_unlock(&_bug68_mutex1);
        pthread_cond_signal(&_bug68_cond1);
    }
    else {
        pthread_mutex_lock(&_bug68_mutex1);
        if (!_bug68_pred)
            pthread_cond_wait(&_bug68_cond1, &_bug68_mutex1);
        if (_bug68_ptr == NULL)
            trigger = 1;
        _bug68_ptr = NULL;
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
    static const uint32_t indices[] = {0x31ecf3};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x300440d2};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118301e};
    static racebench_before_helper before_helpers[2] = {};
    _bug68_input_helper.interleave_num = 2;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0xd11c70f0;
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
static volatile __attribute__((used)) int _bug69_first_part;
static void _bug69_pattern_clean(void)
{
    pthread_mutex_init(&_bug69_mutex1, NULL);
    _bug69_first_part = -1;
}
static __attribute__((optimize(0))) int _bug69_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug69_mutex1);
        if (_bug69_first_part == -1)
            _bug69_first_part = part;
        else
            _bug69_first_part = -1;
        pthread_mutex_unlock(&_bug69_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug69_mutex1);
        if (_bug69_first_part == -1) {
            _bug69_first_part = part;
            trigger = 1;
        }
        else
            _bug69_first_part = -1;
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
    static const uint32_t indices[] = {0x99dd8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xefc57201, 0x95128702};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1153d4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug69_input_helper.interleave_num = 3;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0x25ed3651;
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
static volatile __attribute__((used)) int _bug70_pred;
static volatile __attribute__((used)) int _bug70_sent;
static void _bug70_pattern_clean(void)
{
    pthread_mutex_init(&_bug70_mutex1, NULL);
    pthread_cond_init(&_bug70_cond1, NULL);
    _bug70_pred = 0;
    _bug70_sent = 0;
}
static __attribute__((optimize(0))) int _bug70_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug70_mutex1);
        _bug70_pred = 1;
        pthread_cond_signal(&_bug70_cond1);
        _bug70_sent = 1;
        pthread_mutex_unlock(&_bug70_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug70_mutex1);
        if (_bug70_sent) {
            trigger = 1;
        }
        else {
            while (_bug70_pred == 0)
                pthread_cond_wait(&_bug70_cond1, &_bug70_mutex1);
        }
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
    static const uint32_t indices[] = {0x367de};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1160ade};
    static racebench_before_helper before_helpers[0] = {};
    _bug70_input_helper.interleave_num = 0;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0xa1160ade;
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
static volatile __attribute__((used)) int _bug71_arr[3];
static volatile __attribute__((used)) int _bug71_val;
static volatile __attribute__((used)) int _bug71_pred;
static void _bug71_pattern_clean(void)
{
    _bug71_arr[0] = 0, _bug71_arr[1] = 2, _bug71_arr[2] = 1;
    _bug71_val = 0;
    _bug71_pred = 0;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    int trigger = 0;
    _bug71_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug71_val += _bug71_arr[i];
        }
        __sync_synchronize();
        while (!_bug71_pred)
            pthread_yield();
        trigger = _bug71_val != 3;
        _bug71_pred = 0;
        _bug71_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug71_val += _bug71_arr[i];
        }
        __sync_synchronize();
        _bug71_pred = 1;
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
    static const uint32_t indices[] = {0x37281f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb7037d68};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cae};
    static racebench_before_helper before_helpers[2] = {};
    _bug71_input_helper.interleave_num = 2;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0x581baa16;
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
static volatile __attribute__((used)) int _bug72_val;
static void _bug72_pattern_clean(void)
{
    pthread_mutex_init(&_bug72_mutex1, NULL);
    _bug72_val = 0;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug72_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug72_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug72_mutex1);
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

static void _bug72_pattern_clean(void);
static int _bug72_pattern(int part);
static racebench_bug_helper _bug72_bug_helper;
static racebench_input_helper _bug72_input_helper;
__attribute__((constructor))
static void _bug72_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1a5c7c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182f6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug72_input_helper.interleave_num = 0;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xa1182f6e;
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
    static const uint32_t indices[] = {0x2d87f1};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1150c7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug73_input_helper.interleave_num = 0;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0xa1150c7e;
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
    static const uint32_t indices[] = {0x31f3a5};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug74_input_helper.interleave_num = 0;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int* volatile _bug75_ptr;
static volatile __attribute__((used)) int _bug75_val;
static void _bug75_pattern_clean(void)
{
    pthread_mutex_init(&_bug75_mutex1, NULL);
    _bug75_ptr = NULL;
    _bug75_val = 0;
}
static __attribute__((optimize(0))) int _bug75_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug75_mutex1);
        _bug75_ptr = &_bug75_val;
        pthread_mutex_unlock(&_bug75_mutex1);
        pthread_mutex_lock(&_bug75_mutex1);
        if (_bug75_ptr == NULL)
            trigger = 1;
        _bug75_ptr = NULL;
        pthread_mutex_unlock(&_bug75_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug75_mutex1);
        _bug75_ptr = &_bug75_val;
        if (_bug75_ptr == NULL)
            trigger = 1;
        _bug75_ptr = NULL;
        pthread_mutex_unlock(&_bug75_mutex1);
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
    static const uint32_t indices[] = {0x3ea97};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x3100d359};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1141cbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug75_input_helper.interleave_num = 2;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xd214f017;
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
static volatile __attribute__((used)) int _bug76_ok;
static volatile __attribute__((used)) int* volatile _bug76_ptr;
static volatile __attribute__((used)) int _bug76_val;
static void _bug76_pattern_clean(void)
{
    _bug76_ok = 0;
    _bug76_ptr = NULL;
    _bug76_val = 0;
}
static __attribute__((optimize(0))) int _bug76_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug76_ok = 1;
        __sync_synchronize();
        _bug76_ptr = &_bug76_val;
    }
    else {
        while (!_bug76_ok)
            pthread_yield();
        if (_bug76_ptr == NULL)
            trigger = 1;
        _bug76_ptr = NULL;
        _bug76_ok = 0;
    }
    return trigger;
}

static void _bug76_pattern_clean(void);
static int _bug76_pattern(int part);
static racebench_bug_helper _bug76_bug_helper;
static racebench_input_helper _bug76_input_helper;
__attribute__((constructor))
static void _bug76_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1e2ca1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x3306db3f};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182a3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0xd41f057d;
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
    static const uint32_t indices[] = {0x52267};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa116140e};
    static racebench_before_helper before_helpers[0] = {};
    _bug77_input_helper.interleave_num = 0;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0xa116140e;
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
static pthread_mutex_t _bug78_mutex2;
static void _bug78_pattern_clean(void)
{
    pthread_mutex_init(&_bug78_mutex1, NULL);
    pthread_mutex_init(&_bug78_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug78_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug78_mutex1);
        if (pthread_mutex_trylock(&_bug78_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug78_mutex2);
        pthread_mutex_unlock(&_bug78_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug78_mutex2);
        if (pthread_mutex_trylock(&_bug78_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug78_mutex1);
        pthread_mutex_unlock(&_bug78_mutex2);
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
    static const uint32_t indices[] = {0x300887};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa475a55c};
    static racebench_before_helper before_helpers[1] = {};
    _bug78_input_helper.interleave_num = 1;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0xa475a55c;
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
    static const uint32_t indices[] = {0x2fcbf0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xebb5adc5};
    static const uint8_t before_parts[] = {0x1, 0x0};
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
    _bug79_input_helper.magic_value = 0x8ccdda93;
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
static volatile __attribute__((used)) int _bug80_ok;
static volatile __attribute__((used)) int* volatile _bug80_ptr;
static volatile __attribute__((used)) int _bug80_val;
static void _bug80_pattern_clean(void)
{
    _bug80_ok = 0;
    _bug80_ptr = NULL;
    _bug80_val = 0;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug80_ok = 1;
        __sync_synchronize();
        _bug80_ptr = &_bug80_val;
    }
    else {
        while (!_bug80_ok)
            pthread_yield();
        if (_bug80_ptr == NULL)
            trigger = 1;
        _bug80_ptr = NULL;
        _bug80_ok = 0;
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
    static const uint32_t indices[] = {0x11c06e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x15370168};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1161bae};
    static racebench_before_helper before_helpers[2] = {};
    _bug80_input_helper.interleave_num = 2;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xb64d1d16;
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
static pthread_cond_t _bug81_cond1;
static volatile __attribute__((used)) int* volatile _bug81_ptr;
static volatile __attribute__((used)) int _bug81_val;
static volatile __attribute__((used)) int _bug81_pred;
static void _bug81_pattern_clean(void)
{
    pthread_mutex_init(&_bug81_mutex1, NULL);
    pthread_cond_init(&_bug81_cond1, NULL);
    _bug81_ptr = NULL;
    _bug81_val = 0;
    _bug81_pred = 0;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug81_pred = 0;
        pthread_cond_signal(&_bug81_cond1);
        pthread_mutex_lock(&_bug81_mutex1);
        _bug81_ptr = &_bug81_val;
        _bug81_pred = 1;
        pthread_mutex_unlock(&_bug81_mutex1);
        pthread_cond_signal(&_bug81_cond1);
    }
    else {
        pthread_mutex_lock(&_bug81_mutex1);
        if (!_bug81_pred)
            pthread_cond_wait(&_bug81_cond1, &_bug81_mutex1);
        if (_bug81_ptr == NULL)
            trigger = 1;
        _bug81_ptr = NULL;
        _bug81_pred = 0;
        pthread_mutex_unlock(&_bug81_mutex1);
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
    static const uint32_t indices[] = {0x2763dd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1131dce};
    static racebench_before_helper before_helpers[1] = {};
    _bug81_input_helper.interleave_num = 1;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xa1131dce;
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
static volatile __attribute__((used)) int* volatile _bug82_ptr;
static volatile __attribute__((used)) int _bug82_val;
static void _bug82_pattern_clean(void)
{
    pthread_mutex_init(&_bug82_mutex1, NULL);
    _bug82_ptr = NULL;
    _bug82_val = 0;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug82_mutex1);
        _bug82_ptr = &_bug82_val;
        pthread_mutex_unlock(&_bug82_mutex1);
        pthread_mutex_lock(&_bug82_mutex1);
        if (_bug82_ptr == NULL)
            trigger = 1;
        _bug82_ptr = NULL;
        pthread_mutex_unlock(&_bug82_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug82_mutex1);
        _bug82_ptr = &_bug82_val;
        if (_bug82_ptr == NULL)
            trigger = 1;
        _bug82_ptr = NULL;
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
    static const uint32_t indices[] = {0x2bcae3};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa111fd6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug82_input_helper.interleave_num = 1;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0xa111fd6e;
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
static pthread_mutex_t _bug83_mutex2;
static void _bug83_pattern_clean(void)
{
    pthread_mutex_init(&_bug83_mutex1, NULL);
    pthread_mutex_init(&_bug83_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug83_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug83_mutex1);
        if (pthread_mutex_trylock(&_bug83_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug83_mutex2);
        pthread_mutex_unlock(&_bug83_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug83_mutex2);
        if (pthread_mutex_trylock(&_bug83_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug83_mutex1);
        pthread_mutex_unlock(&_bug83_mutex2);
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
    static const uint32_t indices[] = {0xc896a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x63c38dfc};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xe4514cce};
    static racebench_before_helper before_helpers[2] = {};
    _bug83_input_helper.interleave_num = 2;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0x4814daca;
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
static volatile __attribute__((used)) int _bug84_val;
static void _bug84_pattern_clean(void)
{
    pthread_mutex_init(&_bug84_mutex1, NULL);
    _bug84_val = 0;
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug84_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug84_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug84_mutex1);
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

static void _bug84_pattern_clean(void);
static int _bug84_pattern(int part);
static racebench_bug_helper _bug84_bug_helper;
static racebench_input_helper _bug84_input_helper;
__attribute__((constructor))
static void _bug84_input_helper_init(void)
{
    static const uint32_t indices[] = {0x44700};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xe475813f};
    static racebench_before_helper before_helpers[0] = {};
    _bug84_input_helper.interleave_num = 0;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xe475813f;
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
static pthread_cond_t _bug85_cond1;
static volatile __attribute__((used)) int _bug85_pred;
static volatile __attribute__((used)) int _bug85_sent;
static void _bug85_pattern_clean(void)
{
    pthread_mutex_init(&_bug85_mutex1, NULL);
    pthread_cond_init(&_bug85_cond1, NULL);
    _bug85_pred = 0;
    _bug85_sent = 0;
}
static __attribute__((optimize(0))) int _bug85_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug85_mutex1);
        _bug85_pred = 1;
        pthread_cond_signal(&_bug85_cond1);
        _bug85_sent = 1;
        pthread_mutex_unlock(&_bug85_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug85_mutex1);
        if (_bug85_sent) {
            trigger = 1;
        }
        else {
            while (_bug85_pred == 0)
                pthread_cond_wait(&_bug85_cond1, &_bug85_mutex1);
        }
        _bug85_pred = 0;
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
    static const uint32_t indices[] = {0x2860bc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x5119127d, 0xe5484f11};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x33e5fc5f};
    static racebench_before_helper before_helpers[3] = {};
    _bug85_input_helper.interleave_num = 3;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0x6a475ded;
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
static pthread_mutex_t _bug86_mutex1;
static pthread_mutex_t _bug86_mutex2;
static void _bug86_pattern_clean(void)
{
    pthread_mutex_init(&_bug86_mutex1, NULL);
    pthread_mutex_init(&_bug86_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug86_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug86_mutex1);
        if (pthread_mutex_trylock(&_bug86_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug86_mutex2);
        pthread_mutex_unlock(&_bug86_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug86_mutex2);
        if (pthread_mutex_trylock(&_bug86_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug86_mutex1);
        pthread_mutex_unlock(&_bug86_mutex2);
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
    static const uint32_t indices[] = {0x37ef3d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xb9062ef9};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115626e};
    static racebench_before_helper before_helpers[2] = {};
    _bug86_input_helper.interleave_num = 2;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x5a1b9167;
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
static pthread_mutex_t _bug87_mutex2;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    pthread_mutex_init(&_bug87_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug87_mutex1);
        if (pthread_mutex_trylock(&_bug87_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug87_mutex2);
        pthread_mutex_unlock(&_bug87_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug87_mutex2);
        if (pthread_mutex_trylock(&_bug87_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug87_mutex1);
        pthread_mutex_unlock(&_bug87_mutex2);
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
    static const uint32_t indices[] = {0x178db7};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x834da03a};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118297e};
    static racebench_before_helper before_helpers[2] = {};
    _bug87_input_helper.interleave_num = 2;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0x2465c9b8;
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
static volatile __attribute__((used)) int _bug88_arr[3];
static volatile __attribute__((used)) int _bug88_val;
static volatile __attribute__((used)) int _bug88_pred;
static void _bug88_pattern_clean(void)
{
    _bug88_arr[0] = 0, _bug88_arr[1] = 2, _bug88_arr[2] = 1;
    _bug88_val = 0;
    _bug88_pred = 0;
}
static __attribute__((optimize(0))) int _bug88_pattern(int part)
{
    int trigger = 0;
    _bug88_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug88_val += _bug88_arr[i];
        }
        __sync_synchronize();
        while (!_bug88_pred)
            pthread_yield();
        trigger = _bug88_val != 3;
        _bug88_pred = 0;
        _bug88_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug88_val += _bug88_arr[i];
        }
        __sync_synchronize();
        _bug88_pred = 1;
    }
    return trigger;
}

static void _bug88_pattern_clean(void);
static int _bug88_pattern(int part);
static racebench_bug_helper _bug88_bug_helper;
static racebench_input_helper _bug88_input_helper;
__attribute__((constructor))
static void _bug88_input_helper_init(void)
{
    static const uint32_t indices[] = {0x291dee};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb85c2a00};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xe3f5feab};
    static racebench_before_helper before_helpers[2] = {};
    _bug88_input_helper.interleave_num = 2;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0x9c5228ab;
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
static pthread_mutex_t _bug89_mutex2;
static void _bug89_pattern_clean(void)
{
    pthread_mutex_init(&_bug89_mutex1, NULL);
    pthread_mutex_init(&_bug89_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug89_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug89_mutex1);
        if (pthread_mutex_trylock(&_bug89_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug89_mutex2);
        pthread_mutex_unlock(&_bug89_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug89_mutex2);
        if (pthread_mutex_trylock(&_bug89_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug89_mutex1);
        pthread_mutex_unlock(&_bug89_mutex2);
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
    static const uint32_t indices[] = {0x257c8d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4ce22de8, 0x822cc600};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113530e};
    static racebench_before_helper before_helpers[3] = {};
    _bug89_input_helper.interleave_num = 3;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0x702246f6;
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
static pthread_mutex_t _bug90_mutex1;
static volatile __attribute__((used)) int _bug90_val;
static void _bug90_pattern_clean(void)
{
    pthread_mutex_init(&_bug90_mutex1, NULL);
    _bug90_val = 0;
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug90_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug90_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug90_mutex1);
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

static void _bug90_pattern_clean(void);
static int _bug90_pattern(int part);
static racebench_bug_helper _bug90_bug_helper;
static racebench_input_helper _bug90_input_helper;
__attribute__((constructor))
static void _bug90_input_helper_init(void)
{
    static const uint32_t indices[] = {0x256a1b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xefb24476, 0xdeb85b4e};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118288e};
    static racebench_before_helper before_helpers[3] = {};
    _bug90_input_helper.interleave_num = 3;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0x6f82c852;
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
static volatile __attribute__((used)) int _bug91_ok;
static volatile __attribute__((used)) int* volatile _bug91_ptr;
static volatile __attribute__((used)) int _bug91_val;
static void _bug91_pattern_clean(void)
{
    _bug91_ok = 0;
    _bug91_ptr = NULL;
    _bug91_val = 0;
}
static __attribute__((optimize(0))) int _bug91_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug91_ok = 1;
        __sync_synchronize();
        _bug91_ptr = &_bug91_val;
    }
    else {
        while (!_bug91_ok)
            pthread_yield();
        if (_bug91_ptr == NULL)
            trigger = 1;
        _bug91_ptr = NULL;
        _bug91_ok = 0;
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
    static const uint32_t indices[] = {0x37991b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1120cae};
    static racebench_before_helper before_helpers[0] = {};
    _bug91_input_helper.interleave_num = 0;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0xa1120cae;
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
static pthread_mutex_t _bug92_mutex1;
static volatile __attribute__((used)) int* volatile _bug92_ptr;
static volatile __attribute__((used)) int _bug92_val;
static void _bug92_pattern_clean(void)
{
    pthread_mutex_init(&_bug92_mutex1, NULL);
    _bug92_ptr = NULL;
    _bug92_val = 0;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug92_mutex1);
        _bug92_ptr = &_bug92_val;
        pthread_mutex_unlock(&_bug92_mutex1);
        pthread_mutex_lock(&_bug92_mutex1);
        if (_bug92_ptr == NULL)
            trigger = 1;
        _bug92_ptr = NULL;
        pthread_mutex_unlock(&_bug92_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug92_mutex1);
        _bug92_ptr = &_bug92_val;
        if (_bug92_ptr == NULL)
            trigger = 1;
        _bug92_ptr = NULL;
        pthread_mutex_unlock(&_bug92_mutex1);
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
    static const uint32_t indices[] = {0x378341};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa116bb2e};
    static racebench_before_helper before_helpers[0] = {};
    _bug92_input_helper.interleave_num = 0;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0xa116bb2e;
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
static pthread_mutex_t _bug93_mutex1;
static volatile __attribute__((used)) int* volatile _bug93_ptr;
static volatile __attribute__((used)) int _bug93_val;
static void _bug93_pattern_clean(void)
{
    pthread_mutex_init(&_bug93_mutex1, NULL);
    _bug93_ptr = NULL;
    _bug93_val = 0;
}
static __attribute__((optimize(0))) int _bug93_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug93_mutex1);
        _bug93_ptr = &_bug93_val;
        pthread_mutex_unlock(&_bug93_mutex1);
        pthread_mutex_lock(&_bug93_mutex1);
        if (_bug93_ptr == NULL)
            trigger = 1;
        _bug93_ptr = NULL;
        pthread_mutex_unlock(&_bug93_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug93_mutex1);
        _bug93_ptr = &_bug93_val;
        if (_bug93_ptr == NULL)
            trigger = 1;
        _bug93_ptr = NULL;
        pthread_mutex_unlock(&_bug93_mutex1);
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
    static const uint32_t indices[] = {0xd0d3f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182d7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug93_input_helper.interleave_num = 0;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0xa1182d7e;
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
    static const uint32_t indices[] = {0x203ec2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x42e0403a, 0x4f649317};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa118303e};
    static racebench_before_helper before_helpers[3] = {};
    _bug94_input_helper.interleave_num = 3;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0x335d038f;
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
static volatile __attribute__((used)) int _bug95_ok;
static volatile __attribute__((used)) int* volatile _bug95_ptr;
static volatile __attribute__((used)) int _bug95_val;
static void _bug95_pattern_clean(void)
{
    _bug95_ok = 0;
    _bug95_ptr = NULL;
    _bug95_val = 0;
}
static __attribute__((optimize(0))) int _bug95_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug95_ok = 1;
        __sync_synchronize();
        _bug95_ptr = &_bug95_val;
    }
    else {
        while (!_bug95_ok)
            pthread_yield();
        if (_bug95_ptr == NULL)
            trigger = 1;
        _bug95_ptr = NULL;
        _bug95_ok = 0;
    }
    return trigger;
}

static void _bug95_pattern_clean(void);
static int _bug95_pattern(int part);
static racebench_bug_helper _bug95_bug_helper;
static racebench_input_helper _bug95_input_helper;
__attribute__((constructor))
static void _bug95_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1966c6};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xee25bb9f};
    static racebench_before_helper before_helpers[1] = {};
    _bug95_input_helper.interleave_num = 1;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xee25bb9f;
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
static volatile __attribute__((used)) int _bug96_first_part;
static void _bug96_pattern_clean(void)
{
    pthread_mutex_init(&_bug96_mutex1, NULL);
    _bug96_first_part = -1;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug96_mutex1);
        if (_bug96_first_part == -1)
            _bug96_first_part = part;
        else
            _bug96_first_part = -1;
        pthread_mutex_unlock(&_bug96_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug96_mutex1);
        if (_bug96_first_part == -1) {
            _bug96_first_part = part;
            trigger = 1;
        }
        else
            _bug96_first_part = -1;
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
    static const uint32_t indices[] = {0x80d27};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1189dae};
    static racebench_before_helper before_helpers[0] = {};
    _bug96_input_helper.interleave_num = 0;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xa1189dae;
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
static volatile __attribute__((used)) int _bug97_first_part;
static volatile __attribute__((used)) int _bug97_released;
static void _bug97_pattern_clean(void)
{
    pthread_mutex_init(&_bug97_mutex1, NULL);
    _bug97_first_part = -1;
    _bug97_released = -1;
}
static __attribute__((optimize(0))) int _bug97_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug97_mutex1) == 0)
                break;
            if (_bug97_released == 0) {
                _bug97_first_part = -1;
                _bug97_released = -1;
                return 1;
            }
        }
        if (_bug97_first_part == -1)
            _bug97_first_part = part;
        else {
            _bug97_first_part = -1;
            while (_bug97_released == -1) {}
            _bug97_released = -1;
        }
        pthread_mutex_unlock(&_bug97_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug97_mutex1);
        if (_bug97_first_part == -1) {
            _bug97_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug97_mutex1);
                _bug97_released = 1;
            }
            else {
                _bug97_released = 0;
                while (_bug97_released != -1) {}
                pthread_mutex_unlock(&_bug97_mutex1);
                return 1;
            }
        }
        else {
            _bug97_first_part = -1;
            pthread_mutex_unlock(&_bug97_mutex1);
        }
    }
    return 0;
}

static void _bug97_pattern_clean(void);
static int _bug97_pattern(int part);
static racebench_bug_helper _bug97_bug_helper;
static racebench_input_helper _bug97_input_helper;
__attribute__((constructor))
static void _bug97_input_helper_init(void)
{
    static const uint32_t indices[] = {0x198a00};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf4069c6f};
    static racebench_before_helper before_helpers[0] = {};
    _bug97_input_helper.interleave_num = 0;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0xf4069c6f;
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
static pthread_mutex_t _bug98_mutex1;
static volatile __attribute__((used)) int* volatile _bug98_ptr;
static volatile __attribute__((used)) int _bug98_val;
static void _bug98_pattern_clean(void)
{
    pthread_mutex_init(&_bug98_mutex1, NULL);
    _bug98_ptr = NULL;
    _bug98_val = 0;
}
static __attribute__((optimize(0))) int _bug98_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug98_mutex1);
        _bug98_ptr = &_bug98_val;
        pthread_mutex_unlock(&_bug98_mutex1);
        pthread_mutex_lock(&_bug98_mutex1);
        if (_bug98_ptr == NULL)
            trigger = 1;
        _bug98_ptr = NULL;
        pthread_mutex_unlock(&_bug98_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug98_mutex1);
        _bug98_ptr = &_bug98_val;
        if (_bug98_ptr == NULL)
            trigger = 1;
        _bug98_ptr = NULL;
        pthread_mutex_unlock(&_bug98_mutex1);
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
    static const uint32_t indices[] = {0x372f94};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf00e469c, 0x3dde2f4a};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x54453fca};
    static racebench_before_helper before_helpers[3] = {};
    _bug98_input_helper.interleave_num = 3;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0x8231b5b0;
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
    static const uint32_t indices[] = {0x232ffc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x74159655};
    static racebench_before_helper before_helpers[1] = {};
    _bug99_input_helper.interleave_num = 1;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0x74159655;
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
