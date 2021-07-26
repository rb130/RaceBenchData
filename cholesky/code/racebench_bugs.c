#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "racebench.h"
static void _bug0_pattern_clean(void);
static int _bug0_pattern(int part);
static volatile __attribute__((used)) int _bug0_arr[3];
static volatile __attribute__((used)) int _bug0_val;
static volatile __attribute__((used)) int _bug0_pred;
static void _bug0_pattern_clean(void)
{
    _bug0_arr[0] = 0, _bug0_arr[1] = 2, _bug0_arr[2] = 1;
    _bug0_val = 0;
    _bug0_pred = 0;
}
static __attribute__((optimize(0))) int _bug0_pattern(int part)
{
    int trigger = 0;
    _bug0_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug0_val += _bug0_arr[i];
        }
        __sync_synchronize();
        while (!_bug0_pred)
            pthread_yield();
        trigger = _bug0_val != 3;
        _bug0_pred = 0;
        _bug0_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug0_val += _bug0_arr[i];
        }
        __sync_synchronize();
        _bug0_pred = 1;
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
    static const uint32_t indices[] = {0x2e46};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x256812fd, 0x875696c6};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1151c7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug0_input_helper.interleave_num = 3;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0x4dd3c641;
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
static volatile __attribute__((used)) int _bug1_val;
static void _bug1_pattern_clean(void)
{
    pthread_mutex_init(&_bug1_mutex1, NULL);
    _bug1_val = 0;
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug1_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug1_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug1_mutex1);
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

static void _bug1_pattern_clean(void);
static int _bug1_pattern(int part);
static racebench_bug_helper _bug1_bug_helper;
static racebench_input_helper _bug1_input_helper;
__attribute__((constructor))
static void _bug1_input_helper_init(void)
{
    static const uint32_t indices[] = {0x19a79};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xe052c6ab, 0xc4f503f0};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1118cae};
    static racebench_before_helper before_helpers[3] = {};
    _bug1_input_helper.interleave_num = 3;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0x46595749;
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
    static const uint32_t indices[] = {0x1292c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1147c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug2_input_helper.interleave_num = 1;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0xa1147c4e;
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
static pthread_mutex_t _bug3_mutex1;
static pthread_cond_t _bug3_cond1;
static volatile __attribute__((used)) int* volatile _bug3_ptr;
static volatile __attribute__((used)) int _bug3_val;
static volatile __attribute__((used)) int _bug3_pred;
static void _bug3_pattern_clean(void)
{
    pthread_mutex_init(&_bug3_mutex1, NULL);
    pthread_cond_init(&_bug3_cond1, NULL);
    _bug3_ptr = NULL;
    _bug3_val = 0;
    _bug3_pred = 0;
}
static __attribute__((optimize(0))) int _bug3_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug3_pred = 0;
        pthread_cond_signal(&_bug3_cond1);
        pthread_mutex_lock(&_bug3_mutex1);
        _bug3_ptr = &_bug3_val;
        _bug3_pred = 1;
        pthread_mutex_unlock(&_bug3_mutex1);
        pthread_cond_signal(&_bug3_cond1);
    }
    else {
        pthread_mutex_lock(&_bug3_mutex1);
        if (!_bug3_pred)
            pthread_cond_wait(&_bug3_cond1, &_bug3_mutex1);
        if (_bug3_ptr == NULL)
            trigger = 1;
        _bug3_ptr = NULL;
        _bug3_pred = 0;
        pthread_mutex_unlock(&_bug3_mutex1);
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
    static const uint32_t indices[] = {0x16986};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xedf7a6e3, 0x5ccb0773};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115ec6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug3_input_helper.interleave_num = 3;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xebd89ac4;
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
static void _bug4_pattern_clean(void)
{
    pthread_mutex_init(&_bug4_mutex1, NULL);
    _bug4_first_part = -1;
}
static __attribute__((optimize(0))) int _bug4_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug4_mutex1);
        if (_bug4_first_part == -1)
            _bug4_first_part = part;
        else
            _bug4_first_part = -1;
        pthread_mutex_unlock(&_bug4_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug4_mutex1);
        if (_bug4_first_part == -1) {
            _bug4_first_part = part;
            trigger = 1;
        }
        else
            _bug4_first_part = -1;
        pthread_mutex_unlock(&_bug4_mutex1);
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
    static const uint32_t indices[] = {0xe629};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xd6a5bf9f};
    static racebench_before_helper before_helpers[0] = {};
    _bug4_input_helper.interleave_num = 0;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xd6a5bf9f;
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
static volatile __attribute__((used)) int _bug5_first_part;
static volatile __attribute__((used)) int _bug5_released;
static void _bug5_pattern_clean(void)
{
    pthread_mutex_init(&_bug5_mutex1, NULL);
    _bug5_first_part = -1;
    _bug5_released = -1;
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug5_mutex1) == 0)
                break;
            if (_bug5_released == 0) {
                _bug5_first_part = -1;
                _bug5_released = -1;
                return 1;
            }
        }
        if (_bug5_first_part == -1)
            _bug5_first_part = part;
        else {
            _bug5_first_part = -1;
            while (_bug5_released == -1) {}
            _bug5_released = -1;
        }
        pthread_mutex_unlock(&_bug5_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug5_mutex1);
        if (_bug5_first_part == -1) {
            _bug5_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug5_mutex1);
                _bug5_released = 1;
            }
            else {
                _bug5_released = 0;
                while (_bug5_released != -1) {}
                pthread_mutex_unlock(&_bug5_mutex1);
                return 1;
            }
        }
        else {
            _bug5_first_part = -1;
            pthread_mutex_unlock(&_bug5_mutex1);
        }
    }
    return 0;
}

static void _bug5_pattern_clean(void);
static int _bug5_pattern(int part);
static racebench_bug_helper _bug5_bug_helper;
static racebench_input_helper _bug5_input_helper;
__attribute__((constructor))
static void _bug5_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2ae2c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x11f2a1e1, 0x182db42b};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182bbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug5_input_helper.interleave_num = 3;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xcb3881ca;
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
static volatile __attribute__((used)) int* volatile _bug6_ptr;
static volatile __attribute__((used)) int _bug6_val;
static void _bug6_pattern_clean(void)
{
    pthread_mutex_init(&_bug6_mutex1, NULL);
    _bug6_ptr = NULL;
    _bug6_val = 0;
}
static __attribute__((optimize(0))) int _bug6_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug6_mutex1);
        _bug6_ptr = &_bug6_val;
        pthread_mutex_unlock(&_bug6_mutex1);
        pthread_mutex_lock(&_bug6_mutex1);
        if (_bug6_ptr == NULL)
            trigger = 1;
        _bug6_ptr = NULL;
        pthread_mutex_unlock(&_bug6_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug6_mutex1);
        _bug6_ptr = &_bug6_val;
        if (_bug6_ptr == NULL)
            trigger = 1;
        _bug6_ptr = NULL;
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
    static const uint32_t indices[] = {0x236a6};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa113d00e};
    static racebench_before_helper before_helpers[0] = {};
    _bug6_input_helper.interleave_num = 0;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0xa113d00e;
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
static volatile __attribute__((used)) int _bug7_first_part;
static volatile __attribute__((used)) int _bug7_released;
static void _bug7_pattern_clean(void)
{
    pthread_mutex_init(&_bug7_mutex1, NULL);
    _bug7_first_part = -1;
    _bug7_released = -1;
}
static __attribute__((optimize(0))) int _bug7_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug7_mutex1) == 0)
                break;
            if (_bug7_released == 0) {
                _bug7_first_part = -1;
                _bug7_released = -1;
                return 1;
            }
        }
        if (_bug7_first_part == -1)
            _bug7_first_part = part;
        else {
            _bug7_first_part = -1;
            while (_bug7_released == -1) {}
            _bug7_released = -1;
        }
        pthread_mutex_unlock(&_bug7_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug7_mutex1);
        if (_bug7_first_part == -1) {
            _bug7_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug7_mutex1);
                _bug7_released = 1;
            }
            else {
                _bug7_released = 0;
                while (_bug7_released != -1) {}
                pthread_mutex_unlock(&_bug7_mutex1);
                return 1;
            }
        }
        else {
            _bug7_first_part = -1;
            pthread_mutex_unlock(&_bug7_mutex1);
        }
    }
    return 0;
}

static void _bug7_pattern_clean(void);
static int _bug7_pattern(int part);
static racebench_bug_helper _bug7_bug_helper;
static racebench_input_helper _bug7_input_helper;
__attribute__((constructor))
static void _bug7_input_helper_init(void)
{
    static const uint32_t indices[] = {0xf950};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1156dbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug7_input_helper.interleave_num = 1;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xa1156dbe;
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
    static const uint32_t indices[] = {0x54a2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118304e};
    static racebench_before_helper before_helpers[0] = {};
    _bug8_input_helper.interleave_num = 0;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0xa118304e;
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
static volatile __attribute__((used)) int _bug9_arr[3];
static volatile __attribute__((used)) int _bug9_val;
static volatile __attribute__((used)) int _bug9_pred;
static void _bug9_pattern_clean(void)
{
    _bug9_arr[0] = 0, _bug9_arr[1] = 2, _bug9_arr[2] = 1;
    _bug9_val = 0;
    _bug9_pred = 0;
}
static __attribute__((optimize(0))) int _bug9_pattern(int part)
{
    int trigger = 0;
    _bug9_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug9_val += _bug9_arr[i];
        }
        __sync_synchronize();
        while (!_bug9_pred)
            pthread_yield();
        trigger = _bug9_val != 3;
        _bug9_pred = 0;
        _bug9_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug9_val += _bug9_arr[i];
        }
        __sync_synchronize();
        _bug9_pred = 1;
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
    static const uint32_t indices[] = {0x49c3};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xd775bc5f};
    static racebench_before_helper before_helpers[1] = {};
    _bug9_input_helper.interleave_num = 1;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0xd775bc5f;
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
static volatile __attribute__((used)) int _bug10_first_part;
static volatile __attribute__((used)) int _bug10_released;
static void _bug10_pattern_clean(void)
{
    pthread_mutex_init(&_bug10_mutex1, NULL);
    _bug10_first_part = -1;
    _bug10_released = -1;
}
static __attribute__((optimize(0))) int _bug10_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug10_mutex1) == 0)
                break;
            if (_bug10_released == 0) {
                _bug10_first_part = -1;
                _bug10_released = -1;
                return 1;
            }
        }
        if (_bug10_first_part == -1)
            _bug10_first_part = part;
        else {
            _bug10_first_part = -1;
            while (_bug10_released == -1) {}
            _bug10_released = -1;
        }
        pthread_mutex_unlock(&_bug10_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug10_mutex1);
        if (_bug10_first_part == -1) {
            _bug10_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug10_mutex1);
                _bug10_released = 1;
            }
            else {
                _bug10_released = 0;
                while (_bug10_released != -1) {}
                pthread_mutex_unlock(&_bug10_mutex1);
                return 1;
            }
        }
        else {
            _bug10_first_part = -1;
            pthread_mutex_unlock(&_bug10_mutex1);
        }
    }
    return 0;
}

static void _bug10_pattern_clean(void);
static int _bug10_pattern(int part);
static racebench_bug_helper _bug10_bug_helper;
static racebench_input_helper _bug10_input_helper;
__attribute__((constructor))
static void _bug10_input_helper_init(void)
{
    static const uint32_t indices[] = {0x160ff};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb13263ad, 0xcc608a53};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cae};
    static racebench_before_helper before_helpers[3] = {};
    _bug10_input_helper.interleave_num = 3;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0x1eab1aae;
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
static volatile __attribute__((used)) int _bug11_val;
static void _bug11_pattern_clean(void)
{
    pthread_mutex_init(&_bug11_mutex1, NULL);
    _bug11_val = 0;
}
static __attribute__((optimize(0))) int _bug11_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug11_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug11_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug11_mutex1);
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

static void _bug11_pattern_clean(void);
static int _bug11_pattern(int part);
static racebench_bug_helper _bug11_bug_helper;
static racebench_input_helper _bug11_input_helper;
__attribute__((constructor))
static void _bug11_input_helper_init(void)
{
    static const uint32_t indices[] = {0x7cab};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x50c4d028};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug11_input_helper.interleave_num = 2;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0xf1dcfc96;
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
static pthread_cond_t _bug12_cond1;
static volatile __attribute__((used)) int _bug12_pred;
static volatile __attribute__((used)) int _bug12_sent;
static void _bug12_pattern_clean(void)
{
    pthread_mutex_init(&_bug12_mutex1, NULL);
    pthread_cond_init(&_bug12_cond1, NULL);
    _bug12_pred = 0;
    _bug12_sent = 0;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug12_mutex1);
        _bug12_pred = 1;
        pthread_cond_signal(&_bug12_cond1);
        _bug12_sent = 1;
        pthread_mutex_unlock(&_bug12_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug12_mutex1);
        if (_bug12_sent) {
            trigger = 1;
        }
        else {
            while (_bug12_pred == 0)
                pthread_cond_wait(&_bug12_cond1, &_bug12_mutex1);
        }
        _bug12_pred = 0;
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
    static const uint32_t indices[] = {0x240da};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[1] = {};
    _bug12_input_helper.interleave_num = 1;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xa1182c8e;
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
static volatile __attribute__((used)) int _bug13_first_part;
static volatile __attribute__((used)) int _bug13_released;
static void _bug13_pattern_clean(void)
{
    pthread_mutex_init(&_bug13_mutex1, NULL);
    _bug13_first_part = -1;
    _bug13_released = -1;
}
static __attribute__((optimize(0))) int _bug13_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug13_mutex1) == 0)
                break;
            if (_bug13_released == 0) {
                _bug13_first_part = -1;
                _bug13_released = -1;
                return 1;
            }
        }
        if (_bug13_first_part == -1)
            _bug13_first_part = part;
        else {
            _bug13_first_part = -1;
            while (_bug13_released == -1) {}
            _bug13_released = -1;
        }
        pthread_mutex_unlock(&_bug13_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug13_mutex1);
        if (_bug13_first_part == -1) {
            _bug13_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug13_mutex1);
                _bug13_released = 1;
            }
            else {
                _bug13_released = 0;
                while (_bug13_released != -1) {}
                pthread_mutex_unlock(&_bug13_mutex1);
                return 1;
            }
        }
        else {
            _bug13_first_part = -1;
            pthread_mutex_unlock(&_bug13_mutex1);
        }
    }
    return 0;
}

static void _bug13_pattern_clean(void);
static int _bug13_pattern(int part);
static racebench_bug_helper _bug13_bug_helper;
static racebench_input_helper _bug13_input_helper;
__attribute__((constructor))
static void _bug13_input_helper_init(void)
{
    static const uint32_t indices[] = {0x76dd};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xd35314bc, 0x674f51d5};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182e1e};
    static racebench_before_helper before_helpers[3] = {};
    _bug13_input_helper.interleave_num = 3;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0xdbba94af;
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
    static const uint32_t indices[] = {0x26006};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182afe};
    static racebench_before_helper before_helpers[0] = {};
    _bug14_input_helper.interleave_num = 0;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xa1182afe;
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
static volatile __attribute__((used)) int _bug15_val;
static void _bug15_pattern_clean(void)
{
    pthread_mutex_init(&_bug15_mutex1, NULL);
    _bug15_val = 0;
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug15_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug15_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug15_mutex1);
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

static void _bug15_pattern_clean(void);
static int _bug15_pattern(int part);
static racebench_bug_helper _bug15_bug_helper;
static racebench_input_helper _bug15_input_helper;
__attribute__((constructor))
static void _bug15_input_helper_init(void)
{
    static const uint32_t indices[] = {0x24281};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xe41193a0};
    static racebench_before_helper before_helpers[1] = {};
    _bug15_input_helper.interleave_num = 1;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0xe41193a0;
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
    static const uint32_t indices[] = {0xf2cd};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x9b0487ba, 0xfc8ebd46};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x54444c6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug16_input_helper.interleave_num = 3;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0xebd7916e;
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
static volatile __attribute__((used)) int _bug17_arr[3];
static volatile __attribute__((used)) int _bug17_val;
static volatile __attribute__((used)) int _bug17_pred;
static void _bug17_pattern_clean(void)
{
    _bug17_arr[0] = 0, _bug17_arr[1] = 2, _bug17_arr[2] = 1;
    _bug17_val = 0;
    _bug17_pred = 0;
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    int trigger = 0;
    _bug17_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug17_val += _bug17_arr[i];
        }
        __sync_synchronize();
        while (!_bug17_pred)
            pthread_yield();
        trigger = _bug17_val != 3;
        _bug17_pred = 0;
        _bug17_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug17_val += _bug17_arr[i];
        }
        __sync_synchronize();
        _bug17_pred = 1;
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
    static const uint32_t indices[] = {0x10390};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1150cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug17_input_helper.interleave_num = 0;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0xa1150cbe;
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
static pthread_mutex_t _bug18_mutex2;
static void _bug18_pattern_clean(void)
{
    pthread_mutex_init(&_bug18_mutex1, NULL);
    pthread_mutex_init(&_bug18_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug18_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug18_mutex1);
        if (pthread_mutex_trylock(&_bug18_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug18_mutex2);
        pthread_mutex_unlock(&_bug18_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug18_mutex2);
        if (pthread_mutex_trylock(&_bug18_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug18_mutex1);
        pthread_mutex_unlock(&_bug18_mutex2);
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
    static const uint32_t indices[] = {0x3b0a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1154c5e};
    static racebench_before_helper before_helpers[0] = {};
    _bug18_input_helper.interleave_num = 0;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0xa1154c5e;
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
static pthread_cond_t _bug19_cond1;
static volatile __attribute__((used)) int* volatile _bug19_ptr;
static volatile __attribute__((used)) int _bug19_val;
static volatile __attribute__((used)) int _bug19_pred;
static void _bug19_pattern_clean(void)
{
    pthread_mutex_init(&_bug19_mutex1, NULL);
    pthread_cond_init(&_bug19_cond1, NULL);
    _bug19_ptr = NULL;
    _bug19_val = 0;
    _bug19_pred = 0;
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug19_pred = 0;
        pthread_cond_signal(&_bug19_cond1);
        pthread_mutex_lock(&_bug19_mutex1);
        _bug19_ptr = &_bug19_val;
        _bug19_pred = 1;
        pthread_mutex_unlock(&_bug19_mutex1);
        pthread_cond_signal(&_bug19_cond1);
    }
    else {
        pthread_mutex_lock(&_bug19_mutex1);
        if (!_bug19_pred)
            pthread_cond_wait(&_bug19_cond1, &_bug19_mutex1);
        if (_bug19_ptr == NULL)
            trigger = 1;
        _bug19_ptr = NULL;
        _bug19_pred = 0;
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
    static const uint32_t indices[] = {0x204fe};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa115acbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug19_input_helper.interleave_num = 1;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0xa115acbe;
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
    static const uint32_t indices[] = {0x287e4};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118da9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug20_input_helper.interleave_num = 0;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xa118da9e;
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
static volatile __attribute__((used)) int _bug21_ok;
static volatile __attribute__((used)) int* volatile _bug21_ptr;
static volatile __attribute__((used)) int _bug21_val;
static void _bug21_pattern_clean(void)
{
    _bug21_ok = 0;
    _bug21_ptr = NULL;
    _bug21_val = 0;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug21_ok = 1;
        __sync_synchronize();
        _bug21_ptr = &_bug21_val;
    }
    else {
        while (!_bug21_ok)
            pthread_yield();
        if (_bug21_ptr == NULL)
            trigger = 1;
        _bug21_ptr = NULL;
        _bug21_ok = 0;
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
    static const uint32_t indices[] = {0x275e2};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x94a04c6f};
    static racebench_before_helper before_helpers[1] = {};
    _bug21_input_helper.interleave_num = 1;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0x94a04c6f;
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
    static const uint32_t indices[] = {0x25db1};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa2441e20, 0xa4ee6d29};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x5485bbb0};
    static racebench_before_helper before_helpers[3] = {};
    _bug22_input_helper.interleave_num = 3;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0x9bb846f9;
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
    static const uint32_t indices[] = {0xac9c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xa5b98f95, 0x4d2e3dd8};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113dcbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug23_input_helper.interleave_num = 3;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0x93fbaa2b;
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
static pthread_cond_t _bug24_cond1;
static volatile __attribute__((used)) int _bug24_pred;
static volatile __attribute__((used)) int _bug24_sent;
static void _bug24_pattern_clean(void)
{
    pthread_mutex_init(&_bug24_mutex1, NULL);
    pthread_cond_init(&_bug24_cond1, NULL);
    _bug24_pred = 0;
    _bug24_sent = 0;
}
static __attribute__((optimize(0))) int _bug24_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug24_mutex1);
        _bug24_pred = 1;
        pthread_cond_signal(&_bug24_cond1);
        _bug24_sent = 1;
        pthread_mutex_unlock(&_bug24_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug24_mutex1);
        if (_bug24_sent) {
            trigger = 1;
        }
        else {
            while (_bug24_pred == 0)
                pthread_cond_wait(&_bug24_cond1, &_bug24_mutex1);
        }
        _bug24_pred = 0;
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
    static const uint32_t indices[] = {0x1be92};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xbfc2bd21, 0x830a47f};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x24355caf};
    static racebench_before_helper before_helpers[3] = {};
    _bug24_input_helper.interleave_num = 3;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0xec28be4f;
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
static volatile __attribute__((used)) int* volatile _bug25_ptr;
static volatile __attribute__((used)) int _bug25_val;
static void _bug25_pattern_clean(void)
{
    pthread_mutex_init(&_bug25_mutex1, NULL);
    _bug25_ptr = NULL;
    _bug25_val = 0;
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug25_mutex1);
        _bug25_ptr = &_bug25_val;
        pthread_mutex_unlock(&_bug25_mutex1);
        pthread_mutex_lock(&_bug25_mutex1);
        if (_bug25_ptr == NULL)
            trigger = 1;
        _bug25_ptr = NULL;
        pthread_mutex_unlock(&_bug25_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug25_mutex1);
        _bug25_ptr = &_bug25_val;
        if (_bug25_ptr == NULL)
            trigger = 1;
        _bug25_ptr = NULL;
        pthread_mutex_unlock(&_bug25_mutex1);
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
    static const uint32_t indices[] = {0xe739};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182ade};
    static racebench_before_helper before_helpers[0] = {};
    _bug25_input_helper.interleave_num = 0;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xa1182ade;
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
    static const uint32_t indices[] = {0x19fdc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xdb686783};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xf52bacdb};
    static racebench_before_helper before_helpers[2] = {};
    _bug26_input_helper.interleave_num = 2;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0xd094145e;
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
static void _bug27_pattern_clean(void)
{
    pthread_mutex_init(&_bug27_mutex1, NULL);
    _bug27_first_part = -1;
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug27_mutex1);
        if (_bug27_first_part == -1)
            _bug27_first_part = part;
        else
            _bug27_first_part = -1;
        pthread_mutex_unlock(&_bug27_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug27_mutex1);
        if (_bug27_first_part == -1) {
            _bug27_first_part = part;
            trigger = 1;
        }
        else
            _bug27_first_part = -1;
        pthread_mutex_unlock(&_bug27_mutex1);
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
    static const uint32_t indices[] = {0x19507};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182b4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug27_input_helper.interleave_num = 0;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0xa1182b4e;
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
static void _bug28_pattern_clean(void)
{
    pthread_mutex_init(&_bug28_mutex1, NULL);
    _bug28_first_part = -1;
}
static __attribute__((optimize(0))) int _bug28_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug28_mutex1);
        if (_bug28_first_part == -1)
            _bug28_first_part = part;
        else
            _bug28_first_part = -1;
        pthread_mutex_unlock(&_bug28_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug28_mutex1);
        if (_bug28_first_part == -1) {
            _bug28_first_part = part;
            trigger = 1;
        }
        else
            _bug28_first_part = -1;
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
    static const uint32_t indices[] = {0x18c82};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182fee};
    static racebench_before_helper before_helpers[0] = {};
    _bug28_input_helper.interleave_num = 0;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa1182fee;
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
static pthread_mutex_t _bug29_mutex2;
static void _bug29_pattern_clean(void)
{
    pthread_mutex_init(&_bug29_mutex1, NULL);
    pthread_mutex_init(&_bug29_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug29_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug29_mutex1);
        if (pthread_mutex_trylock(&_bug29_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug29_mutex2);
        pthread_mutex_unlock(&_bug29_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug29_mutex2);
        if (pthread_mutex_trylock(&_bug29_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug29_mutex1);
        pthread_mutex_unlock(&_bug29_mutex2);
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
    static const uint32_t indices[] = {0x1324a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1155f5e};
    static racebench_before_helper before_helpers[0] = {};
    _bug29_input_helper.interleave_num = 0;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0xa1155f5e;
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
static volatile __attribute__((used)) int _bug30_first_part;
static volatile __attribute__((used)) int _bug30_released;
static void _bug30_pattern_clean(void)
{
    pthread_mutex_init(&_bug30_mutex1, NULL);
    _bug30_first_part = -1;
    _bug30_released = -1;
}
static __attribute__((optimize(0))) int _bug30_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug30_mutex1) == 0)
                break;
            if (_bug30_released == 0) {
                _bug30_first_part = -1;
                _bug30_released = -1;
                return 1;
            }
        }
        if (_bug30_first_part == -1)
            _bug30_first_part = part;
        else {
            _bug30_first_part = -1;
            while (_bug30_released == -1) {}
            _bug30_released = -1;
        }
        pthread_mutex_unlock(&_bug30_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug30_mutex1);
        if (_bug30_first_part == -1) {
            _bug30_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug30_mutex1);
                _bug30_released = 1;
            }
            else {
                _bug30_released = 0;
                while (_bug30_released != -1) {}
                pthread_mutex_unlock(&_bug30_mutex1);
                return 1;
            }
        }
        else {
            _bug30_first_part = -1;
            pthread_mutex_unlock(&_bug30_mutex1);
        }
    }
    return 0;
}

static void _bug30_pattern_clean(void);
static int _bug30_pattern(int part);
static racebench_bug_helper _bug30_bug_helper;
static racebench_input_helper _bug30_input_helper;
__attribute__((constructor))
static void _bug30_input_helper_init(void)
{
    static const uint32_t indices[] = {0x16067};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1126cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug30_input_helper.interleave_num = 0;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0xa1126cbe;
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
static void _bug31_pattern_clean(void)
{
    pthread_mutex_init(&_bug31_mutex1, NULL);
    _bug31_first_part = -1;
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug31_mutex1);
        if (_bug31_first_part == -1)
            _bug31_first_part = part;
        else
            _bug31_first_part = -1;
        pthread_mutex_unlock(&_bug31_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug31_mutex1);
        if (_bug31_first_part == -1) {
            _bug31_first_part = part;
            trigger = 1;
        }
        else
            _bug31_first_part = -1;
        pthread_mutex_unlock(&_bug31_mutex1);
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
    static const uint32_t indices[] = {0x39e4};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xcb850ab4, 0x81c11a77};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x24566cbf};
    static racebench_before_helper before_helpers[3] = {};
    _bug31_input_helper.interleave_num = 3;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0x719c91ea;
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
static pthread_cond_t _bug32_cond1;
static volatile __attribute__((used)) int* volatile _bug32_ptr;
static volatile __attribute__((used)) int _bug32_val;
static volatile __attribute__((used)) int _bug32_pred;
static void _bug32_pattern_clean(void)
{
    pthread_mutex_init(&_bug32_mutex1, NULL);
    pthread_cond_init(&_bug32_cond1, NULL);
    _bug32_ptr = NULL;
    _bug32_val = 0;
    _bug32_pred = 0;
}
static __attribute__((optimize(0))) int _bug32_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug32_pred = 0;
        pthread_cond_signal(&_bug32_cond1);
        pthread_mutex_lock(&_bug32_mutex1);
        _bug32_ptr = &_bug32_val;
        _bug32_pred = 1;
        pthread_mutex_unlock(&_bug32_mutex1);
        pthread_cond_signal(&_bug32_cond1);
    }
    else {
        pthread_mutex_lock(&_bug32_mutex1);
        if (!_bug32_pred)
            pthread_cond_wait(&_bug32_cond1, &_bug32_mutex1);
        if (_bug32_ptr == NULL)
            trigger = 1;
        _bug32_ptr = NULL;
        _bug32_pred = 0;
        pthread_mutex_unlock(&_bug32_mutex1);
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
    static const uint32_t indices[] = {0x25eff};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x13ab9c13, 0x629729fb};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11830ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug32_input_helper.interleave_num = 3;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0x175af6bc;
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
static pthread_mutex_t _bug33_mutex1;
static volatile __attribute__((used)) int* volatile _bug33_ptr;
static volatile __attribute__((used)) int _bug33_val;
static void _bug33_pattern_clean(void)
{
    pthread_mutex_init(&_bug33_mutex1, NULL);
    _bug33_ptr = NULL;
    _bug33_val = 0;
}
static __attribute__((optimize(0))) int _bug33_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug33_mutex1);
        _bug33_ptr = &_bug33_val;
        pthread_mutex_unlock(&_bug33_mutex1);
        pthread_mutex_lock(&_bug33_mutex1);
        if (_bug33_ptr == NULL)
            trigger = 1;
        _bug33_ptr = NULL;
        pthread_mutex_unlock(&_bug33_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug33_mutex1);
        _bug33_ptr = &_bug33_val;
        if (_bug33_ptr == NULL)
            trigger = 1;
        _bug33_ptr = NULL;
        pthread_mutex_unlock(&_bug33_mutex1);
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
    static const uint32_t indices[] = {0x14485};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x559712ef};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xf4626cbf};
    static racebench_before_helper before_helpers[2] = {};
    _bug33_input_helper.interleave_num = 2;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0x49f97fae;
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
static pthread_cond_t _bug34_cond1;
static volatile __attribute__((used)) int* volatile _bug34_ptr;
static volatile __attribute__((used)) int _bug34_val;
static volatile __attribute__((used)) int _bug34_pred;
static void _bug34_pattern_clean(void)
{
    pthread_mutex_init(&_bug34_mutex1, NULL);
    pthread_cond_init(&_bug34_cond1, NULL);
    _bug34_ptr = NULL;
    _bug34_val = 0;
    _bug34_pred = 0;
}
static __attribute__((optimize(0))) int _bug34_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug34_pred = 0;
        pthread_cond_signal(&_bug34_cond1);
        pthread_mutex_lock(&_bug34_mutex1);
        _bug34_ptr = &_bug34_val;
        _bug34_pred = 1;
        pthread_mutex_unlock(&_bug34_mutex1);
        pthread_cond_signal(&_bug34_cond1);
    }
    else {
        pthread_mutex_lock(&_bug34_mutex1);
        if (!_bug34_pred)
            pthread_cond_wait(&_bug34_cond1, &_bug34_mutex1);
        if (_bug34_ptr == NULL)
            trigger = 1;
        _bug34_ptr = NULL;
        _bug34_pred = 0;
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
    static const uint32_t indices[] = {0x184af};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11829ee};
    static racebench_before_helper before_helpers[1] = {};
    _bug34_input_helper.interleave_num = 1;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0xa11829ee;
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
    static const uint32_t indices[] = {0x5ad1};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112f05e};
    static racebench_before_helper before_helpers[0] = {};
    _bug35_input_helper.interleave_num = 0;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xa112f05e;
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
static volatile __attribute__((used)) int _bug36_arr[3];
static volatile __attribute__((used)) int _bug36_val;
static volatile __attribute__((used)) int _bug36_pred;
static void _bug36_pattern_clean(void)
{
    _bug36_arr[0] = 0, _bug36_arr[1] = 2, _bug36_arr[2] = 1;
    _bug36_val = 0;
    _bug36_pred = 0;
}
static __attribute__((optimize(0))) int _bug36_pattern(int part)
{
    int trigger = 0;
    _bug36_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug36_val += _bug36_arr[i];
        }
        __sync_synchronize();
        while (!_bug36_pred)
            pthread_yield();
        trigger = _bug36_val != 3;
        _bug36_pred = 0;
        _bug36_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug36_val += _bug36_arr[i];
        }
        __sync_synchronize();
        _bug36_pred = 1;
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
    static const uint32_t indices[] = {0x1ae25};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug36_input_helper.interleave_num = 0;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int _bug37_ok;
static volatile __attribute__((used)) int* volatile _bug37_ptr;
static volatile __attribute__((used)) int _bug37_val;
static void _bug37_pattern_clean(void)
{
    _bug37_ok = 0;
    _bug37_ptr = NULL;
    _bug37_val = 0;
}
static __attribute__((optimize(0))) int _bug37_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug37_ok = 1;
        __sync_synchronize();
        _bug37_ptr = &_bug37_val;
    }
    else {
        while (!_bug37_ok)
            pthread_yield();
        if (_bug37_ptr == NULL)
            trigger = 1;
        _bug37_ptr = NULL;
        _bug37_ok = 0;
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
    static const uint32_t indices[] = {0xe1b5};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x9fc0687};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa113ad7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug37_input_helper.interleave_num = 2;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0xab0fb405;
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
static pthread_cond_t _bug38_cond1;
static volatile __attribute__((used)) int _bug38_pred;
static volatile __attribute__((used)) int _bug38_sent;
static void _bug38_pattern_clean(void)
{
    pthread_mutex_init(&_bug38_mutex1, NULL);
    pthread_cond_init(&_bug38_cond1, NULL);
    _bug38_pred = 0;
    _bug38_sent = 0;
}
static __attribute__((optimize(0))) int _bug38_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug38_mutex1);
        _bug38_pred = 1;
        pthread_cond_signal(&_bug38_cond1);
        _bug38_sent = 1;
        pthread_mutex_unlock(&_bug38_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug38_mutex1);
        if (_bug38_sent) {
            trigger = 1;
        }
        else {
            while (_bug38_pred == 0)
                pthread_cond_wait(&_bug38_cond1, &_bug38_mutex1);
        }
        _bug38_pred = 0;
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
    static const uint32_t indices[] = {0x26810};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[0] = {};
    _bug38_input_helper.interleave_num = 0;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xa1182cce;
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
    static const uint32_t indices[] = {0x293fc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf118fee5};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x1419cccf};
    static racebench_before_helper before_helpers[2] = {};
    _bug39_input_helper.interleave_num = 2;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0x532cbb4;
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
    static const uint32_t indices[] = {0x11190};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug40_input_helper.interleave_num = 0;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int _bug41_ok;
static volatile __attribute__((used)) int* volatile _bug41_ptr;
static volatile __attribute__((used)) int _bug41_val;
static void _bug41_pattern_clean(void)
{
    _bug41_ok = 0;
    _bug41_ptr = NULL;
    _bug41_val = 0;
}
static __attribute__((optimize(0))) int _bug41_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug41_ok = 1;
        __sync_synchronize();
        _bug41_ptr = &_bug41_val;
    }
    else {
        while (!_bug41_ok)
            pthread_yield();
        if (_bug41_ptr == NULL)
            trigger = 1;
        _bug41_ptr = NULL;
        _bug41_ok = 0;
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
    static const uint32_t indices[] = {0x1d932};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa111eafe};
    static racebench_before_helper before_helpers[1] = {};
    _bug41_input_helper.interleave_num = 1;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa111eafe;
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
    static const uint32_t indices[] = {0x19715};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1117d3e};
    static racebench_before_helper before_helpers[1] = {};
    _bug42_input_helper.interleave_num = 1;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0xa1117d3e;
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
static pthread_mutex_t _bug43_mutex1;
static pthread_cond_t _bug43_cond1;
static volatile __attribute__((used)) int* volatile _bug43_ptr;
static volatile __attribute__((used)) int _bug43_val;
static volatile __attribute__((used)) int _bug43_pred;
static void _bug43_pattern_clean(void)
{
    pthread_mutex_init(&_bug43_mutex1, NULL);
    pthread_cond_init(&_bug43_cond1, NULL);
    _bug43_ptr = NULL;
    _bug43_val = 0;
    _bug43_pred = 0;
}
static __attribute__((optimize(0))) int _bug43_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug43_pred = 0;
        pthread_cond_signal(&_bug43_cond1);
        pthread_mutex_lock(&_bug43_mutex1);
        _bug43_ptr = &_bug43_val;
        _bug43_pred = 1;
        pthread_mutex_unlock(&_bug43_mutex1);
        pthread_cond_signal(&_bug43_cond1);
    }
    else {
        pthread_mutex_lock(&_bug43_mutex1);
        if (!_bug43_pred)
            pthread_cond_wait(&_bug43_cond1, &_bug43_mutex1);
        if (_bug43_ptr == NULL)
            trigger = 1;
        _bug43_ptr = NULL;
        _bug43_pred = 0;
        pthread_mutex_unlock(&_bug43_mutex1);
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
    static const uint32_t indices[] = {0x27a82};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112605e};
    static racebench_before_helper before_helpers[0] = {};
    _bug43_input_helper.interleave_num = 0;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0xa112605e;
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
static volatile __attribute__((used)) int* volatile _bug44_ptr;
static volatile __attribute__((used)) int _bug44_val;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    _bug44_ptr = NULL;
    _bug44_val = 0;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug44_mutex1);
        _bug44_ptr = &_bug44_val;
        pthread_mutex_unlock(&_bug44_mutex1);
        pthread_mutex_lock(&_bug44_mutex1);
        if (_bug44_ptr == NULL)
            trigger = 1;
        _bug44_ptr = NULL;
        pthread_mutex_unlock(&_bug44_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug44_mutex1);
        _bug44_ptr = &_bug44_val;
        if (_bug44_ptr == NULL)
            trigger = 1;
        _bug44_ptr = NULL;
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
    static const uint32_t indices[] = {0xbf24};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x76a15164};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug44_input_helper.interleave_num = 2;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0x17b97da2;
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
    static const uint32_t indices[] = {0x3368};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa116170e};
    static racebench_before_helper before_helpers[1] = {};
    _bug45_input_helper.interleave_num = 1;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0xa116170e;
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
static volatile __attribute__((used)) int _bug46_first_part;
static volatile __attribute__((used)) int _bug46_released;
static void _bug46_pattern_clean(void)
{
    pthread_mutex_init(&_bug46_mutex1, NULL);
    _bug46_first_part = -1;
    _bug46_released = -1;
}
static __attribute__((optimize(0))) int _bug46_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug46_mutex1) == 0)
                break;
            if (_bug46_released == 0) {
                _bug46_first_part = -1;
                _bug46_released = -1;
                return 1;
            }
        }
        if (_bug46_first_part == -1)
            _bug46_first_part = part;
        else {
            _bug46_first_part = -1;
            while (_bug46_released == -1) {}
            _bug46_released = -1;
        }
        pthread_mutex_unlock(&_bug46_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug46_mutex1);
        if (_bug46_first_part == -1) {
            _bug46_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug46_mutex1);
                _bug46_released = 1;
            }
            else {
                _bug46_released = 0;
                while (_bug46_released != -1) {}
                pthread_mutex_unlock(&_bug46_mutex1);
                return 1;
            }
        }
        else {
            _bug46_first_part = -1;
            pthread_mutex_unlock(&_bug46_mutex1);
        }
    }
    return 0;
}

static void _bug46_pattern_clean(void);
static int _bug46_pattern(int part);
static racebench_bug_helper _bug46_bug_helper;
static racebench_input_helper _bug46_input_helper;
__attribute__((constructor))
static void _bug46_input_helper_init(void)
{
    static const uint32_t indices[] = {0x206c5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xd4a53caf};
    static racebench_before_helper before_helpers[1] = {};
    _bug46_input_helper.interleave_num = 1;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0xd4a53caf;
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
static pthread_mutex_t _bug47_mutex2;
static void _bug47_pattern_clean(void)
{
    pthread_mutex_init(&_bug47_mutex1, NULL);
    pthread_mutex_init(&_bug47_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug47_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug47_mutex1);
        if (pthread_mutex_trylock(&_bug47_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug47_mutex2);
        pthread_mutex_unlock(&_bug47_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug47_mutex2);
        if (pthread_mutex_trylock(&_bug47_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug47_mutex1);
        pthread_mutex_unlock(&_bug47_mutex2);
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
    static const uint32_t indices[] = {0x1b916};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd1c98a63, 0x124f3fd};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x64e5acac};
    static racebench_before_helper before_helpers[3] = {};
    _bug47_input_helper.interleave_num = 3;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0x37d42b0c;
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
    static const uint32_t indices[] = {0x194d9};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x647471fc, 0x8b27fde2};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cfe};
    static racebench_before_helper before_helpers[3] = {};
    _bug48_input_helper.interleave_num = 3;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0x90b49cdc;
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
static volatile __attribute__((used)) int _bug49_first_part;
static volatile __attribute__((used)) int _bug49_released;
static void _bug49_pattern_clean(void)
{
    pthread_mutex_init(&_bug49_mutex1, NULL);
    _bug49_first_part = -1;
    _bug49_released = -1;
}
static __attribute__((optimize(0))) int _bug49_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug49_mutex1) == 0)
                break;
            if (_bug49_released == 0) {
                _bug49_first_part = -1;
                _bug49_released = -1;
                return 1;
            }
        }
        if (_bug49_first_part == -1)
            _bug49_first_part = part;
        else {
            _bug49_first_part = -1;
            while (_bug49_released == -1) {}
            _bug49_released = -1;
        }
        pthread_mutex_unlock(&_bug49_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug49_mutex1);
        if (_bug49_first_part == -1) {
            _bug49_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug49_mutex1);
                _bug49_released = 1;
            }
            else {
                _bug49_released = 0;
                while (_bug49_released != -1) {}
                pthread_mutex_unlock(&_bug49_mutex1);
                return 1;
            }
        }
        else {
            _bug49_first_part = -1;
            pthread_mutex_unlock(&_bug49_mutex1);
        }
    }
    return 0;
}

static void _bug49_pattern_clean(void);
static int _bug49_pattern(int part);
static racebench_bug_helper _bug49_bug_helper;
static racebench_input_helper _bug49_input_helper;
__attribute__((constructor))
static void _bug49_input_helper_init(void)
{
    static const uint32_t indices[] = {0x250f4};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c3e};
    static racebench_before_helper before_helpers[0] = {};
    _bug49_input_helper.interleave_num = 0;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xa1182c3e;
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
static pthread_mutex_t _bug50_mutex1;
static pthread_cond_t _bug50_cond1;
static volatile __attribute__((used)) int _bug50_pred;
static volatile __attribute__((used)) int _bug50_sent;
static void _bug50_pattern_clean(void)
{
    pthread_mutex_init(&_bug50_mutex1, NULL);
    pthread_cond_init(&_bug50_cond1, NULL);
    _bug50_pred = 0;
    _bug50_sent = 0;
}
static __attribute__((optimize(0))) int _bug50_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug50_mutex1);
        _bug50_pred = 1;
        pthread_cond_signal(&_bug50_cond1);
        _bug50_sent = 1;
        pthread_mutex_unlock(&_bug50_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug50_mutex1);
        if (_bug50_sent) {
            trigger = 1;
        }
        else {
            while (_bug50_pred == 0)
                pthread_cond_wait(&_bug50_cond1, &_bug50_mutex1);
        }
        _bug50_pred = 0;
        pthread_mutex_unlock(&_bug50_mutex1);
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
    static const uint32_t indices[] = {0x4b09};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xe405bc70};
    static racebench_before_helper before_helpers[1] = {};
    _bug50_input_helper.interleave_num = 1;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0xe405bc70;
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
static volatile __attribute__((used)) int _bug51_ok;
static volatile __attribute__((used)) int* volatile _bug51_ptr;
static volatile __attribute__((used)) int _bug51_val;
static void _bug51_pattern_clean(void)
{
    _bug51_ok = 0;
    _bug51_ptr = NULL;
    _bug51_val = 0;
}
static __attribute__((optimize(0))) int _bug51_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug51_ok = 1;
        __sync_synchronize();
        _bug51_ptr = &_bug51_val;
    }
    else {
        while (!_bug51_ok)
            pthread_yield();
        if (_bug51_ptr == NULL)
            trigger = 1;
        _bug51_ptr = NULL;
        _bug51_ok = 0;
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
    static const uint32_t indices[] = {0x1a1b0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x73568db};
    static racebench_before_helper before_helpers[0] = {};
    _bug51_input_helper.interleave_num = 0;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0x73568db;
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
static volatile __attribute__((used)) int _bug52_ok;
static volatile __attribute__((used)) int* volatile _bug52_ptr;
static volatile __attribute__((used)) int _bug52_val;
static void _bug52_pattern_clean(void)
{
    _bug52_ok = 0;
    _bug52_ptr = NULL;
    _bug52_val = 0;
}
static __attribute__((optimize(0))) int _bug52_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug52_ok = 1;
        __sync_synchronize();
        _bug52_ptr = &_bug52_val;
    }
    else {
        while (!_bug52_ok)
            pthread_yield();
        if (_bug52_ptr == NULL)
            trigger = 1;
        _bug52_ptr = NULL;
        _bug52_ok = 0;
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
    static const uint32_t indices[] = {0x1f2a9};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x683d0a19, 0x2b6cfc6};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa111610e};
    static racebench_before_helper before_helpers[3] = {};
    _bug52_input_helper.interleave_num = 3;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0xc053aed;
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
static pthread_cond_t _bug53_cond1;
static volatile __attribute__((used)) int _bug53_pred;
static volatile __attribute__((used)) int _bug53_sent;
static void _bug53_pattern_clean(void)
{
    pthread_mutex_init(&_bug53_mutex1, NULL);
    pthread_cond_init(&_bug53_cond1, NULL);
    _bug53_pred = 0;
    _bug53_sent = 0;
}
static __attribute__((optimize(0))) int _bug53_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug53_mutex1);
        _bug53_pred = 1;
        pthread_cond_signal(&_bug53_cond1);
        _bug53_sent = 1;
        pthread_mutex_unlock(&_bug53_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug53_mutex1);
        if (_bug53_sent) {
            trigger = 1;
        }
        else {
            while (_bug53_pred == 0)
                pthread_cond_wait(&_bug53_cond1, &_bug53_mutex1);
        }
        _bug53_pred = 0;
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
    static const uint32_t indices[] = {0x254b6};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x42d0085f, 0x709c74ea};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11260ee};
    static racebench_before_helper before_helpers[3] = {};
    _bug53_input_helper.interleave_num = 3;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0x547ede37;
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
static volatile __attribute__((used)) int _bug54_released;
static void _bug54_pattern_clean(void)
{
    pthread_mutex_init(&_bug54_mutex1, NULL);
    _bug54_first_part = -1;
    _bug54_released = -1;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug54_mutex1) == 0)
                break;
            if (_bug54_released == 0) {
                _bug54_first_part = -1;
                _bug54_released = -1;
                return 1;
            }
        }
        if (_bug54_first_part == -1)
            _bug54_first_part = part;
        else {
            _bug54_first_part = -1;
            while (_bug54_released == -1) {}
            _bug54_released = -1;
        }
        pthread_mutex_unlock(&_bug54_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug54_mutex1);
        if (_bug54_first_part == -1) {
            _bug54_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug54_mutex1);
                _bug54_released = 1;
            }
            else {
                _bug54_released = 0;
                while (_bug54_released != -1) {}
                pthread_mutex_unlock(&_bug54_mutex1);
                return 1;
            }
        }
        else {
            _bug54_first_part = -1;
            pthread_mutex_unlock(&_bug54_mutex1);
        }
    }
    return 0;
}

static void _bug54_pattern_clean(void);
static int _bug54_pattern(int part);
static racebench_bug_helper _bug54_bug_helper;
static racebench_input_helper _bug54_input_helper;
__attribute__((constructor))
static void _bug54_input_helper_init(void)
{
    static const uint32_t indices[] = {0x23596};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7187c71e};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1126d7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug54_input_helper.interleave_num = 2;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0x129a349c;
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
static volatile __attribute__((used)) int _bug55_val;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    _bug55_val = 0;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug55_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug55_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug55_mutex1);
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

static void _bug55_pattern_clean(void);
static int _bug55_pattern(int part);
static racebench_bug_helper _bug55_bug_helper;
static racebench_input_helper _bug55_input_helper;
__attribute__((constructor))
static void _bug55_input_helper_init(void)
{
    static const uint32_t indices[] = {0x14c25};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x428bccfe, 0x33588b54};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xb4472c7b};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0x2a2b84cd;
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
static pthread_cond_t _bug56_cond1;
static volatile __attribute__((used)) int* volatile _bug56_ptr;
static volatile __attribute__((used)) int _bug56_val;
static volatile __attribute__((used)) int _bug56_pred;
static void _bug56_pattern_clean(void)
{
    pthread_mutex_init(&_bug56_mutex1, NULL);
    pthread_cond_init(&_bug56_cond1, NULL);
    _bug56_ptr = NULL;
    _bug56_val = 0;
    _bug56_pred = 0;
}
static __attribute__((optimize(0))) int _bug56_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug56_pred = 0;
        pthread_cond_signal(&_bug56_cond1);
        pthread_mutex_lock(&_bug56_mutex1);
        _bug56_ptr = &_bug56_val;
        _bug56_pred = 1;
        pthread_mutex_unlock(&_bug56_mutex1);
        pthread_cond_signal(&_bug56_cond1);
    }
    else {
        pthread_mutex_lock(&_bug56_mutex1);
        if (!_bug56_pred)
            pthread_cond_wait(&_bug56_cond1, &_bug56_mutex1);
        if (_bug56_ptr == NULL)
            trigger = 1;
        _bug56_ptr = NULL;
        _bug56_pred = 0;
        pthread_mutex_unlock(&_bug56_mutex1);
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
    static const uint32_t indices[] = {0x18b42};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4e123058};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1122ece};
    static racebench_before_helper before_helpers[2] = {};
    _bug56_input_helper.interleave_num = 2;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0xef245f26;
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
static volatile __attribute__((used)) int _bug57_ok;
static volatile __attribute__((used)) int* volatile _bug57_ptr;
static volatile __attribute__((used)) int _bug57_val;
static void _bug57_pattern_clean(void)
{
    _bug57_ok = 0;
    _bug57_ptr = NULL;
    _bug57_val = 0;
}
static __attribute__((optimize(0))) int _bug57_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug57_ok = 1;
        __sync_synchronize();
        _bug57_ptr = &_bug57_val;
    }
    else {
        while (!_bug57_ok)
            pthread_yield();
        if (_bug57_ptr == NULL)
            trigger = 1;
        _bug57_ptr = NULL;
        _bug57_ok = 0;
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
    static const uint32_t indices[] = {0x28dae};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8f551f6a, 0xe8cc5294};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cfe};
    static racebench_before_helper before_helpers[3] = {};
    _bug57_input_helper.interleave_num = 3;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0x19399efc;
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
static pthread_mutex_t _bug58_mutex2;
static void _bug58_pattern_clean(void)
{
    pthread_mutex_init(&_bug58_mutex1, NULL);
    pthread_mutex_init(&_bug58_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug58_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug58_mutex1);
        if (pthread_mutex_trylock(&_bug58_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug58_mutex2);
        pthread_mutex_unlock(&_bug58_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug58_mutex2);
        if (pthread_mutex_trylock(&_bug58_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug58_mutex1);
        pthread_mutex_unlock(&_bug58_mutex2);
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
    static const uint32_t indices[] = {0x1ee22};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1134c9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug58_input_helper.interleave_num = 0;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0xa1134c9e;
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
static pthread_mutex_t _bug59_mutex2;
static void _bug59_pattern_clean(void)
{
    pthread_mutex_init(&_bug59_mutex1, NULL);
    pthread_mutex_init(&_bug59_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug59_mutex1);
        if (pthread_mutex_trylock(&_bug59_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug59_mutex2);
        pthread_mutex_unlock(&_bug59_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug59_mutex2);
        if (pthread_mutex_trylock(&_bug59_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug59_mutex1);
        pthread_mutex_unlock(&_bug59_mutex2);
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
    static const uint32_t indices[] = {0x9f20};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug59_input_helper.interleave_num = 1;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0xa1182d6e;
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
static volatile __attribute__((used)) int _bug60_ok;
static volatile __attribute__((used)) int* volatile _bug60_ptr;
static volatile __attribute__((used)) int _bug60_val;
static void _bug60_pattern_clean(void)
{
    _bug60_ok = 0;
    _bug60_ptr = NULL;
    _bug60_val = 0;
}
static __attribute__((optimize(0))) int _bug60_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug60_ok = 1;
        __sync_synchronize();
        _bug60_ptr = &_bug60_val;
    }
    else {
        while (!_bug60_ok)
            pthread_yield();
        if (_bug60_ptr == NULL)
            trigger = 1;
        _bug60_ptr = NULL;
        _bug60_ok = 0;
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
    static const uint32_t indices[] = {0x50ae};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xf5b72cb0};
    static racebench_before_helper before_helpers[1] = {};
    _bug60_input_helper.interleave_num = 1;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0xf5b72cb0;
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
static volatile __attribute__((used)) int _bug61_ok;
static volatile __attribute__((used)) int* volatile _bug61_ptr;
static volatile __attribute__((used)) int _bug61_val;
static void _bug61_pattern_clean(void)
{
    _bug61_ok = 0;
    _bug61_ptr = NULL;
    _bug61_val = 0;
}
static __attribute__((optimize(0))) int _bug61_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug61_ok = 1;
        __sync_synchronize();
        _bug61_ptr = &_bug61_val;
    }
    else {
        while (!_bug61_ok)
            pthread_yield();
        if (_bug61_ptr == NULL)
            trigger = 1;
        _bug61_ptr = NULL;
        _bug61_ok = 0;
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
    static const uint32_t indices[] = {0x22e6d};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug61_input_helper.interleave_num = 0;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0xa1182c1e;
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
static pthread_mutex_t _bug62_mutex2;
static void _bug62_pattern_clean(void)
{
    pthread_mutex_init(&_bug62_mutex1, NULL);
    pthread_mutex_init(&_bug62_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug62_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug62_mutex1);
        if (pthread_mutex_trylock(&_bug62_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug62_mutex2);
        pthread_mutex_unlock(&_bug62_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug62_mutex2);
        if (pthread_mutex_trylock(&_bug62_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug62_mutex1);
        pthread_mutex_unlock(&_bug62_mutex2);
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
    static const uint32_t indices[] = {0x1b3b3};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xd3789cd0, 0xc559c4df};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug62_input_helper.interleave_num = 3;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0x39ea8dfd;
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
    static const uint32_t indices[] = {0xe261};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xece415e6};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa116afee};
    static racebench_before_helper before_helpers[2] = {};
    _bug63_input_helper.interleave_num = 2;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x8dfac5d4;
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
static volatile __attribute__((used)) int _bug64_first_part;
static void _bug64_pattern_clean(void)
{
    pthread_mutex_init(&_bug64_mutex1, NULL);
    _bug64_first_part = -1;
}
static __attribute__((optimize(0))) int _bug64_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug64_mutex1);
        if (_bug64_first_part == -1)
            _bug64_first_part = part;
        else
            _bug64_first_part = -1;
        pthread_mutex_unlock(&_bug64_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug64_mutex1);
        if (_bug64_first_part == -1) {
            _bug64_first_part = part;
            trigger = 1;
        }
        else
            _bug64_first_part = -1;
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
    static const uint32_t indices[] = {0x23156};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x80108994};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c9e};
    static racebench_before_helper before_helpers[2] = {};
    _bug64_input_helper.interleave_num = 2;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0x2128b632;
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
static pthread_mutex_t _bug65_mutex2;
static void _bug65_pattern_clean(void)
{
    pthread_mutex_init(&_bug65_mutex1, NULL);
    pthread_mutex_init(&_bug65_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug65_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug65_mutex1);
        if (pthread_mutex_trylock(&_bug65_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug65_mutex2);
        pthread_mutex_unlock(&_bug65_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug65_mutex2);
        if (pthread_mutex_trylock(&_bug65_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug65_mutex1);
        pthread_mutex_unlock(&_bug65_mutex2);
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
    static const uint32_t indices[] = {0x11422};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug65_input_helper.interleave_num = 1;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0xa1182cbe;
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
static pthread_cond_t _bug66_cond1;
static volatile __attribute__((used)) int* volatile _bug66_ptr;
static volatile __attribute__((used)) int _bug66_val;
static volatile __attribute__((used)) int _bug66_pred;
static void _bug66_pattern_clean(void)
{
    pthread_mutex_init(&_bug66_mutex1, NULL);
    pthread_cond_init(&_bug66_cond1, NULL);
    _bug66_ptr = NULL;
    _bug66_val = 0;
    _bug66_pred = 0;
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug66_pred = 0;
        pthread_cond_signal(&_bug66_cond1);
        pthread_mutex_lock(&_bug66_mutex1);
        _bug66_ptr = &_bug66_val;
        _bug66_pred = 1;
        pthread_mutex_unlock(&_bug66_mutex1);
        pthread_cond_signal(&_bug66_cond1);
    }
    else {
        pthread_mutex_lock(&_bug66_mutex1);
        if (!_bug66_pred)
            pthread_cond_wait(&_bug66_cond1, &_bug66_mutex1);
        if (_bug66_ptr == NULL)
            trigger = 1;
        _bug66_ptr = NULL;
        _bug66_pred = 0;
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
    static const uint32_t indices[] = {0x2548a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11639ee};
    static racebench_before_helper before_helpers[0] = {};
    _bug66_input_helper.interleave_num = 0;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0xa11639ee;
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
static volatile __attribute__((used)) int _bug67_ok;
static volatile __attribute__((used)) int* volatile _bug67_ptr;
static volatile __attribute__((used)) int _bug67_val;
static void _bug67_pattern_clean(void)
{
    _bug67_ok = 0;
    _bug67_ptr = NULL;
    _bug67_val = 0;
}
static __attribute__((optimize(0))) int _bug67_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug67_ok = 1;
        __sync_synchronize();
        _bug67_ptr = &_bug67_val;
    }
    else {
        while (!_bug67_ok)
            pthread_yield();
        if (_bug67_ptr == NULL)
            trigger = 1;
        _bug67_ptr = NULL;
        _bug67_ok = 0;
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
    static const uint32_t indices[] = {0x103da};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug67_input_helper.interleave_num = 0;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0xa1182c9e;
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
static volatile __attribute__((used)) int _bug68_first_part;
static void _bug68_pattern_clean(void)
{
    pthread_mutex_init(&_bug68_mutex1, NULL);
    _bug68_first_part = -1;
}
static __attribute__((optimize(0))) int _bug68_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug68_mutex1);
        if (_bug68_first_part == -1)
            _bug68_first_part = part;
        else
            _bug68_first_part = -1;
        pthread_mutex_unlock(&_bug68_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug68_mutex1);
        if (_bug68_first_part == -1) {
            _bug68_first_part = part;
            trigger = 1;
        }
        else
            _bug68_first_part = -1;
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
    static const uint32_t indices[] = {0x23719};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7c179b9c};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa117acce};
    static racebench_before_helper before_helpers[2] = {};
    _bug68_input_helper.interleave_num = 2;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0x1d2f486a;
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
    static const uint32_t indices[] = {0x1af22};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xd5c30dd1, 0x68d411c1};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115f07e};
    static racebench_before_helper before_helpers[3] = {};
    _bug69_input_helper.interleave_num = 3;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0xdfad1010;
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
static volatile __attribute__((used)) int _bug70_arr[3];
static volatile __attribute__((used)) int _bug70_val;
static volatile __attribute__((used)) int _bug70_pred;
static void _bug70_pattern_clean(void)
{
    _bug70_arr[0] = 0, _bug70_arr[1] = 2, _bug70_arr[2] = 1;
    _bug70_val = 0;
    _bug70_pred = 0;
}
static __attribute__((optimize(0))) int _bug70_pattern(int part)
{
    int trigger = 0;
    _bug70_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug70_val += _bug70_arr[i];
        }
        __sync_synchronize();
        while (!_bug70_pred)
            pthread_yield();
        trigger = _bug70_val != 3;
        _bug70_pred = 0;
        _bug70_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug70_val += _bug70_arr[i];
        }
        __sync_synchronize();
        _bug70_pred = 1;
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
    static const uint32_t indices[] = {0x257ba};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x142cacc2};
    static racebench_before_helper before_helpers[0] = {};
    _bug70_input_helper.interleave_num = 0;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0x142cacc2;
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
static volatile __attribute__((used)) int _bug71_first_part;
static volatile __attribute__((used)) int _bug71_released;
static void _bug71_pattern_clean(void)
{
    pthread_mutex_init(&_bug71_mutex1, NULL);
    _bug71_first_part = -1;
    _bug71_released = -1;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug71_mutex1) == 0)
                break;
            if (_bug71_released == 0) {
                _bug71_first_part = -1;
                _bug71_released = -1;
                return 1;
            }
        }
        if (_bug71_first_part == -1)
            _bug71_first_part = part;
        else {
            _bug71_first_part = -1;
            while (_bug71_released == -1) {}
            _bug71_released = -1;
        }
        pthread_mutex_unlock(&_bug71_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug71_mutex1);
        if (_bug71_first_part == -1) {
            _bug71_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug71_mutex1);
                _bug71_released = 1;
            }
            else {
                _bug71_released = 0;
                while (_bug71_released != -1) {}
                pthread_mutex_unlock(&_bug71_mutex1);
                return 1;
            }
        }
        else {
            _bug71_first_part = -1;
            pthread_mutex_unlock(&_bug71_mutex1);
        }
    }
    return 0;
}

static void _bug71_pattern_clean(void);
static int _bug71_pattern(int part);
static racebench_bug_helper _bug71_bug_helper;
static racebench_input_helper _bug71_input_helper;
__attribute__((constructor))
static void _bug71_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2b0f8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x1a654c94};
    static racebench_before_helper before_helpers[0] = {};
    _bug71_input_helper.interleave_num = 0;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0x1a654c94;
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
static pthread_cond_t _bug72_cond1;
static volatile __attribute__((used)) int* volatile _bug72_ptr;
static volatile __attribute__((used)) int _bug72_val;
static volatile __attribute__((used)) int _bug72_pred;
static void _bug72_pattern_clean(void)
{
    pthread_mutex_init(&_bug72_mutex1, NULL);
    pthread_cond_init(&_bug72_cond1, NULL);
    _bug72_ptr = NULL;
    _bug72_val = 0;
    _bug72_pred = 0;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug72_pred = 0;
        pthread_cond_signal(&_bug72_cond1);
        pthread_mutex_lock(&_bug72_mutex1);
        _bug72_ptr = &_bug72_val;
        _bug72_pred = 1;
        pthread_mutex_unlock(&_bug72_mutex1);
        pthread_cond_signal(&_bug72_cond1);
    }
    else {
        pthread_mutex_lock(&_bug72_mutex1);
        if (!_bug72_pred)
            pthread_cond_wait(&_bug72_cond1, &_bug72_mutex1);
        if (_bug72_ptr == NULL)
            trigger = 1;
        _bug72_ptr = NULL;
        _bug72_pred = 0;
        pthread_mutex_unlock(&_bug72_mutex1);
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
    static const uint32_t indices[] = {0x1fb7b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x9f23a37, 0x5f091a6c};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115ac0e};
    static racebench_before_helper before_helpers[3] = {};
    _bug72_input_helper.interleave_num = 3;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xa1100b1;
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
static volatile __attribute__((used)) int _bug73_val;
static void _bug73_pattern_clean(void)
{
    pthread_mutex_init(&_bug73_mutex1, NULL);
    _bug73_val = 0;
}
static __attribute__((optimize(0))) int _bug73_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug73_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug73_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug73_mutex1);
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

static void _bug73_pattern_clean(void);
static int _bug73_pattern(int part);
static racebench_bug_helper _bug73_bug_helper;
static racebench_input_helper _bug73_input_helper;
__attribute__((constructor))
static void _bug73_input_helper_init(void)
{
    static const uint32_t indices[] = {0x233ce};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11e8a2e};
    static racebench_before_helper before_helpers[0] = {};
    _bug73_input_helper.interleave_num = 0;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0xa11e8a2e;
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
static pthread_mutex_t _bug74_mutex1;
static pthread_cond_t _bug74_cond1;
static volatile __attribute__((used)) int* volatile _bug74_ptr;
static volatile __attribute__((used)) int _bug74_val;
static volatile __attribute__((used)) int _bug74_pred;
static void _bug74_pattern_clean(void)
{
    pthread_mutex_init(&_bug74_mutex1, NULL);
    pthread_cond_init(&_bug74_cond1, NULL);
    _bug74_ptr = NULL;
    _bug74_val = 0;
    _bug74_pred = 0;
}
static __attribute__((optimize(0))) int _bug74_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug74_pred = 0;
        pthread_cond_signal(&_bug74_cond1);
        pthread_mutex_lock(&_bug74_mutex1);
        _bug74_ptr = &_bug74_val;
        _bug74_pred = 1;
        pthread_mutex_unlock(&_bug74_mutex1);
        pthread_cond_signal(&_bug74_cond1);
    }
    else {
        pthread_mutex_lock(&_bug74_mutex1);
        if (!_bug74_pred)
            pthread_cond_wait(&_bug74_cond1, &_bug74_mutex1);
        if (_bug74_ptr == NULL)
            trigger = 1;
        _bug74_ptr = NULL;
        _bug74_pred = 0;
        pthread_mutex_unlock(&_bug74_mutex1);
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
    static const uint32_t indices[] = {0xe1b4};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182f0e};
    static racebench_before_helper before_helpers[0] = {};
    _bug74_input_helper.interleave_num = 0;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0xa1182f0e;
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
    static const uint32_t indices[] = {0x20a55};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11560ee};
    static racebench_before_helper before_helpers[1] = {};
    _bug75_input_helper.interleave_num = 1;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xa11560ee;
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
static pthread_cond_t _bug76_cond1;
static volatile __attribute__((used)) int _bug76_pred;
static volatile __attribute__((used)) int _bug76_sent;
static void _bug76_pattern_clean(void)
{
    pthread_mutex_init(&_bug76_mutex1, NULL);
    pthread_cond_init(&_bug76_cond1, NULL);
    _bug76_pred = 0;
    _bug76_sent = 0;
}
static __attribute__((optimize(0))) int _bug76_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug76_mutex1);
        _bug76_pred = 1;
        pthread_cond_signal(&_bug76_cond1);
        _bug76_sent = 1;
        pthread_mutex_unlock(&_bug76_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug76_mutex1);
        if (_bug76_sent) {
            trigger = 1;
        }
        else {
            while (_bug76_pred == 0)
                pthread_cond_wait(&_bug76_cond1, &_bug76_mutex1);
        }
        _bug76_pred = 0;
        pthread_mutex_unlock(&_bug76_mutex1);
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
    static const uint32_t indices[] = {0x27d5a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x6a620e0e, 0xa3e7aebc};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1189dee};
    static racebench_before_helper before_helpers[3] = {};
    _bug76_input_helper.interleave_num = 3;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0xaf625ab8;
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
static volatile __attribute__((used)) int _bug77_ok;
static volatile __attribute__((used)) int* volatile _bug77_ptr;
static volatile __attribute__((used)) int _bug77_val;
static void _bug77_pattern_clean(void)
{
    _bug77_ok = 0;
    _bug77_ptr = NULL;
    _bug77_val = 0;
}
static __attribute__((optimize(0))) int _bug77_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug77_ok = 1;
        __sync_synchronize();
        _bug77_ptr = &_bug77_val;
    }
    else {
        while (!_bug77_ok)
            pthread_yield();
        if (_bug77_ptr == NULL)
            trigger = 1;
        _bug77_ptr = NULL;
        _bug77_ok = 0;
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
    static const uint32_t indices[] = {0x14459};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa115bf3e};
    static racebench_before_helper before_helpers[1] = {};
    _bug77_input_helper.interleave_num = 1;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0xa115bf3e;
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
static volatile __attribute__((used)) int _bug78_first_part;
static void _bug78_pattern_clean(void)
{
    pthread_mutex_init(&_bug78_mutex1, NULL);
    _bug78_first_part = -1;
}
static __attribute__((optimize(0))) int _bug78_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug78_mutex1);
        if (_bug78_first_part == -1)
            _bug78_first_part = part;
        else
            _bug78_first_part = -1;
        pthread_mutex_unlock(&_bug78_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug78_mutex1);
        if (_bug78_first_part == -1) {
            _bug78_first_part = part;
            trigger = 1;
        }
        else
            _bug78_first_part = -1;
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
    static const uint32_t indices[] = {0x2160f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c9e};
    static racebench_before_helper before_helpers[1] = {};
    _bug78_input_helper.interleave_num = 1;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0xa1182c9e;
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
static volatile __attribute__((used)) int _bug79_ok;
static volatile __attribute__((used)) int* volatile _bug79_ptr;
static volatile __attribute__((used)) int _bug79_val;
static void _bug79_pattern_clean(void)
{
    _bug79_ok = 0;
    _bug79_ptr = NULL;
    _bug79_val = 0;
}
static __attribute__((optimize(0))) int _bug79_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug79_ok = 1;
        __sync_synchronize();
        _bug79_ptr = &_bug79_val;
    }
    else {
        while (!_bug79_ok)
            pthread_yield();
        if (_bug79_ptr == NULL)
            trigger = 1;
        _bug79_ptr = NULL;
        _bug79_ok = 0;
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
    static const uint32_t indices[] = {0x13741};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xc108e4f2};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x244afc40};
    static racebench_before_helper before_helpers[2] = {};
    _bug79_input_helper.interleave_num = 2;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0xe553e132;
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
static volatile __attribute__((used)) int _bug80_val;
static void _bug80_pattern_clean(void)
{
    pthread_mutex_init(&_bug80_mutex1, NULL);
    _bug80_val = 0;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug80_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug80_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug80_mutex1);
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

static void _bug80_pattern_clean(void);
static int _bug80_pattern(int part);
static racebench_bug_helper _bug80_bug_helper;
static racebench_input_helper _bug80_input_helper;
__attribute__((constructor))
static void _bug80_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1d9b0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cde};
    static racebench_before_helper before_helpers[0] = {};
    _bug80_input_helper.interleave_num = 0;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xa1182cde;
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
static volatile __attribute__((used)) int _bug81_arr[3];
static volatile __attribute__((used)) int _bug81_val;
static volatile __attribute__((used)) int _bug81_pred;
static void _bug81_pattern_clean(void)
{
    _bug81_arr[0] = 0, _bug81_arr[1] = 2, _bug81_arr[2] = 1;
    _bug81_val = 0;
    _bug81_pred = 0;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    int trigger = 0;
    _bug81_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug81_val += _bug81_arr[i];
        }
        __sync_synchronize();
        while (!_bug81_pred)
            pthread_yield();
        trigger = _bug81_val != 3;
        _bug81_pred = 0;
        _bug81_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug81_val += _bug81_arr[i];
        }
        __sync_synchronize();
        _bug81_pred = 1;
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
    static const uint32_t indices[] = {0x22ce2};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1144cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug81_input_helper.interleave_num = 1;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xa1144cbe;
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
static volatile __attribute__((used)) int _bug82_first_part;
static volatile __attribute__((used)) int _bug82_released;
static void _bug82_pattern_clean(void)
{
    pthread_mutex_init(&_bug82_mutex1, NULL);
    _bug82_first_part = -1;
    _bug82_released = -1;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug82_mutex1) == 0)
                break;
            if (_bug82_released == 0) {
                _bug82_first_part = -1;
                _bug82_released = -1;
                return 1;
            }
        }
        if (_bug82_first_part == -1)
            _bug82_first_part = part;
        else {
            _bug82_first_part = -1;
            while (_bug82_released == -1) {}
            _bug82_released = -1;
        }
        pthread_mutex_unlock(&_bug82_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug82_mutex1);
        if (_bug82_first_part == -1) {
            _bug82_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug82_mutex1);
                _bug82_released = 1;
            }
            else {
                _bug82_released = 0;
                while (_bug82_released != -1) {}
                pthread_mutex_unlock(&_bug82_mutex1);
                return 1;
            }
        }
        else {
            _bug82_first_part = -1;
            pthread_mutex_unlock(&_bug82_mutex1);
        }
    }
    return 0;
}

static void _bug82_pattern_clean(void);
static int _bug82_pattern(int part);
static racebench_bug_helper _bug82_bug_helper;
static racebench_input_helper _bug82_input_helper;
__attribute__((constructor))
static void _bug82_input_helper_init(void)
{
    static const uint32_t indices[] = {0x239d1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182b1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug82_input_helper.interleave_num = 1;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0xa1182b1e;
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
static volatile __attribute__((used)) int _bug83_val;
static void _bug83_pattern_clean(void)
{
    pthread_mutex_init(&_bug83_mutex1, NULL);
    _bug83_val = 0;
}
static __attribute__((optimize(0))) int _bug83_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug83_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug83_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug83_mutex1);
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

static void _bug83_pattern_clean(void);
static int _bug83_pattern(int part);
static racebench_bug_helper _bug83_bug_helper;
static racebench_input_helper _bug83_input_helper;
__attribute__((constructor))
static void _bug83_input_helper_init(void)
{
    static const uint32_t indices[] = {0x12fe9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x2a27cac};
    static racebench_before_helper before_helpers[1] = {};
    _bug83_input_helper.interleave_num = 1;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0x2a27cac;
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
static volatile __attribute__((used)) int _bug84_first_part;
static void _bug84_pattern_clean(void)
{
    pthread_mutex_init(&_bug84_mutex1, NULL);
    _bug84_first_part = -1;
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug84_mutex1);
        if (_bug84_first_part == -1)
            _bug84_first_part = part;
        else
            _bug84_first_part = -1;
        pthread_mutex_unlock(&_bug84_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug84_mutex1);
        if (_bug84_first_part == -1) {
            _bug84_first_part = part;
            trigger = 1;
        }
        else
            _bug84_first_part = -1;
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
    static const uint32_t indices[] = {0x28c75};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182d1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug84_input_helper.interleave_num = 0;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xa1182d1e;
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
static volatile __attribute__((used)) int _bug85_ok;
static volatile __attribute__((used)) int* volatile _bug85_ptr;
static volatile __attribute__((used)) int _bug85_val;
static void _bug85_pattern_clean(void)
{
    _bug85_ok = 0;
    _bug85_ptr = NULL;
    _bug85_val = 0;
}
static __attribute__((optimize(0))) int _bug85_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug85_ok = 1;
        __sync_synchronize();
        _bug85_ptr = &_bug85_val;
    }
    else {
        while (!_bug85_ok)
            pthread_yield();
        if (_bug85_ptr == NULL)
            trigger = 1;
        _bug85_ptr = NULL;
        _bug85_ok = 0;
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
    static const uint32_t indices[] = {0xf463};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x68550c7b};
    static racebench_before_helper before_helpers[0] = {};
    _bug85_input_helper.interleave_num = 0;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0x68550c7b;
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
    static const uint32_t indices[] = {0x19123};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182f2e};
    static racebench_before_helper before_helpers[0] = {};
    _bug86_input_helper.interleave_num = 0;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0xa1182f2e;
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
static volatile __attribute__((used)) int _bug87_val;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    _bug87_val = 0;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug87_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug87_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug87_mutex1);
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

static void _bug87_pattern_clean(void);
static int _bug87_pattern(int part);
static racebench_bug_helper _bug87_bug_helper;
static racebench_input_helper _bug87_input_helper;
__attribute__((constructor))
static void _bug87_input_helper_init(void)
{
    static const uint32_t indices[] = {0x21a60};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[0] = {};
    _bug87_input_helper.interleave_num = 0;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0xa1182cce;
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
    static const uint32_t indices[] = {0x4a7c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug88_input_helper.interleave_num = 0;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0xa1182c4e;
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
    static const uint32_t indices[] = {0xd7ee};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x84210ccd};
    static racebench_before_helper before_helpers[0] = {};
    _bug89_input_helper.interleave_num = 0;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0x84210ccd;
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
static pthread_mutex_t _bug90_mutex2;
static void _bug90_pattern_clean(void)
{
    pthread_mutex_init(&_bug90_mutex1, NULL);
    pthread_mutex_init(&_bug90_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug90_mutex1);
        if (pthread_mutex_trylock(&_bug90_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug90_mutex2);
        pthread_mutex_unlock(&_bug90_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug90_mutex2);
        if (pthread_mutex_trylock(&_bug90_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug90_mutex1);
        pthread_mutex_unlock(&_bug90_mutex2);
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
    static const uint32_t indices[] = {0x293e3};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa6ddf40f, 0xb5e9fbfe};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xb4541c7c};
    static racebench_before_helper before_helpers[3] = {};
    _bug90_input_helper.interleave_num = 3;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0x111c0c89;
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
    static const uint32_t indices[] = {0x6408};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115ecfe};
    static racebench_before_helper before_helpers[0] = {};
    _bug91_input_helper.interleave_num = 0;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0xa115ecfe;
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
    static const uint32_t indices[] = {0x28c48};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x44450e8f};
    static racebench_before_helper before_helpers[1] = {};
    _bug92_input_helper.interleave_num = 1;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0x44450e8f;
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
static volatile __attribute__((used)) int _bug93_arr[3];
static volatile __attribute__((used)) int _bug93_val;
static volatile __attribute__((used)) int _bug93_pred;
static void _bug93_pattern_clean(void)
{
    _bug93_arr[0] = 0, _bug93_arr[1] = 2, _bug93_arr[2] = 1;
    _bug93_val = 0;
    _bug93_pred = 0;
}
static __attribute__((optimize(0))) int _bug93_pattern(int part)
{
    int trigger = 0;
    _bug93_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug93_val += _bug93_arr[i];
        }
        __sync_synchronize();
        while (!_bug93_pred)
            pthread_yield();
        trigger = _bug93_val != 3;
        _bug93_pred = 0;
        _bug93_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug93_val += _bug93_arr[i];
        }
        __sync_synchronize();
        _bug93_pred = 1;
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
    static const uint32_t indices[] = {0x10212};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xdd6873f4};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xc4310cac};
    static racebench_before_helper before_helpers[2] = {};
    _bug93_input_helper.interleave_num = 2;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0xa19980a0;
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
static pthread_mutex_t _bug94_mutex1;
static pthread_cond_t _bug94_cond1;
static volatile __attribute__((used)) int* volatile _bug94_ptr;
static volatile __attribute__((used)) int _bug94_val;
static volatile __attribute__((used)) int _bug94_pred;
static void _bug94_pattern_clean(void)
{
    pthread_mutex_init(&_bug94_mutex1, NULL);
    pthread_cond_init(&_bug94_cond1, NULL);
    _bug94_ptr = NULL;
    _bug94_val = 0;
    _bug94_pred = 0;
}
static __attribute__((optimize(0))) int _bug94_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug94_pred = 0;
        pthread_cond_signal(&_bug94_cond1);
        pthread_mutex_lock(&_bug94_mutex1);
        _bug94_ptr = &_bug94_val;
        _bug94_pred = 1;
        pthread_mutex_unlock(&_bug94_mutex1);
        pthread_cond_signal(&_bug94_cond1);
    }
    else {
        pthread_mutex_lock(&_bug94_mutex1);
        if (!_bug94_pred)
            pthread_cond_wait(&_bug94_cond1, &_bug94_mutex1);
        if (_bug94_ptr == NULL)
            trigger = 1;
        _bug94_ptr = NULL;
        _bug94_pred = 0;
        pthread_mutex_unlock(&_bug94_mutex1);
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
    static const uint32_t indices[] = {0x106ec};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa111ff0e};
    static racebench_before_helper before_helpers[1] = {};
    _bug94_input_helper.interleave_num = 1;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0xa111ff0e;
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
    static const uint32_t indices[] = {0x19f7d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1152c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug95_input_helper.interleave_num = 0;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xa1152c4e;
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
static volatile __attribute__((used)) int _bug96_ok;
static volatile __attribute__((used)) int* volatile _bug96_ptr;
static volatile __attribute__((used)) int _bug96_val;
static void _bug96_pattern_clean(void)
{
    _bug96_ok = 0;
    _bug96_ptr = NULL;
    _bug96_val = 0;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug96_ok = 1;
        __sync_synchronize();
        _bug96_ptr = &_bug96_val;
    }
    else {
        while (!_bug96_ok)
            pthread_yield();
        if (_bug96_ptr == NULL)
            trigger = 1;
        _bug96_ptr = NULL;
        _bug96_ok = 0;
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
    static const uint32_t indices[] = {0xc151};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xf515afdf};
    static racebench_before_helper before_helpers[1] = {};
    _bug96_input_helper.interleave_num = 1;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xf515afdf;
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
static pthread_mutex_t _bug97_mutex2;
static void _bug97_pattern_clean(void)
{
    pthread_mutex_init(&_bug97_mutex1, NULL);
    pthread_mutex_init(&_bug97_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug97_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug97_mutex1);
        if (pthread_mutex_trylock(&_bug97_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug97_mutex2);
        pthread_mutex_unlock(&_bug97_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug97_mutex2);
        if (pthread_mutex_trylock(&_bug97_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug97_mutex1);
        pthread_mutex_unlock(&_bug97_mutex2);
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
    static const uint32_t indices[] = {0x186ca};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd2d4774a, 0x7cad1f3f};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa42f1e5b};
    static racebench_before_helper before_helpers[3] = {};
    _bug97_input_helper.interleave_num = 3;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0xf3b0b4e4;
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
static volatile __attribute__((used)) int _bug98_val;
static void _bug98_pattern_clean(void)
{
    pthread_mutex_init(&_bug98_mutex1, NULL);
    _bug98_val = 0;
}
static __attribute__((optimize(0))) int _bug98_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug98_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug98_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug98_mutex1);
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

static void _bug98_pattern_clean(void);
static int _bug98_pattern(int part);
static racebench_bug_helper _bug98_bug_helper;
static racebench_input_helper _bug98_input_helper;
__attribute__((constructor))
static void _bug98_input_helper_init(void)
{
    static const uint32_t indices[] = {0x11339};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x64de563a, 0xbd45cc26};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa118274e};
    static racebench_before_helper before_helpers[3] = {};
    _bug98_input_helper.interleave_num = 3;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0xc33c49ae;
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
static volatile __attribute__((used)) int _bug99_ok;
static volatile __attribute__((used)) int* volatile _bug99_ptr;
static volatile __attribute__((used)) int _bug99_val;
static void _bug99_pattern_clean(void)
{
    _bug99_ok = 0;
    _bug99_ptr = NULL;
    _bug99_val = 0;
}
static __attribute__((optimize(0))) int _bug99_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug99_ok = 1;
        __sync_synchronize();
        _bug99_ptr = &_bug99_val;
    }
    else {
        while (!_bug99_ok)
            pthread_yield();
        if (_bug99_ptr == NULL)
            trigger = 1;
        _bug99_ptr = NULL;
        _bug99_ok = 0;
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
    static const uint32_t indices[] = {0x1b8b3};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa118274e};
    static racebench_before_helper before_helpers[1] = {};
    _bug99_input_helper.interleave_num = 1;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0xa118274e;
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
