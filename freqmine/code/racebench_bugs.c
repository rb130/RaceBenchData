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
    static const uint32_t indices[] = {0x4e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x8ffea4a};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1163d2e};
    static racebench_before_helper before_helpers[2] = {};
    _bug0_input_helper.interleave_num = 2;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0xaa162778;
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
static pthread_mutex_t _bug1_mutex2;
static void _bug1_pattern_clean(void)
{
    pthread_mutex_init(&_bug1_mutex1, NULL);
    pthread_mutex_init(&_bug1_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug1_mutex1);
        if (pthread_mutex_trylock(&_bug1_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug1_mutex2);
        pthread_mutex_unlock(&_bug1_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug1_mutex2);
        if (pthread_mutex_trylock(&_bug1_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug1_mutex1);
        pthread_mutex_unlock(&_bug1_mutex2);
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
    static const uint32_t indices[] = {0x54};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x5a2a6b6, 0x1aab9a20};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug1_input_helper.interleave_num = 3;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xc1666d94;
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
static volatile __attribute__((used)) int* volatile _bug2_ptr;
static volatile __attribute__((used)) int _bug2_val;
static void _bug2_pattern_clean(void)
{
    pthread_mutex_init(&_bug2_mutex1, NULL);
    _bug2_ptr = NULL;
    _bug2_val = 0;
}
static __attribute__((optimize(0))) int _bug2_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug2_mutex1);
        _bug2_ptr = &_bug2_val;
        pthread_mutex_unlock(&_bug2_mutex1);
        pthread_mutex_lock(&_bug2_mutex1);
        if (_bug2_ptr == NULL)
            trigger = 1;
        _bug2_ptr = NULL;
        pthread_mutex_unlock(&_bug2_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug2_mutex1);
        _bug2_ptr = &_bug2_val;
        if (_bug2_ptr == NULL)
            trigger = 1;
        _bug2_ptr = NULL;
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
    static const uint32_t indices[] = {0x52};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xe983be99};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115bdce};
    static racebench_before_helper before_helpers[2] = {};
    _bug2_input_helper.interleave_num = 2;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0x8a997c67;
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
static volatile __attribute__((used)) int* volatile _bug3_ptr;
static volatile __attribute__((used)) int _bug3_val;
static void _bug3_pattern_clean(void)
{
    pthread_mutex_init(&_bug3_mutex1, NULL);
    _bug3_ptr = NULL;
    _bug3_val = 0;
}
static __attribute__((optimize(0))) int _bug3_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug3_mutex1);
        _bug3_ptr = &_bug3_val;
        pthread_mutex_unlock(&_bug3_mutex1);
        pthread_mutex_lock(&_bug3_mutex1);
        if (_bug3_ptr == NULL)
            trigger = 1;
        _bug3_ptr = NULL;
        pthread_mutex_unlock(&_bug3_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug3_mutex1);
        _bug3_ptr = &_bug3_val;
        if (_bug3_ptr == NULL)
            trigger = 1;
        _bug3_ptr = NULL;
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
    static const uint32_t indices[] = {0x49};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1209cde};
    static racebench_before_helper before_helpers[0] = {};
    _bug3_input_helper.interleave_num = 0;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xa1209cde;
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
static volatile __attribute__((used)) int _bug4_arr[3];
static volatile __attribute__((used)) int _bug4_val;
static volatile __attribute__((used)) int _bug4_pred;
static void _bug4_pattern_clean(void)
{
    _bug4_arr[0] = 0, _bug4_arr[1] = 2, _bug4_arr[2] = 1;
    _bug4_val = 0;
    _bug4_pred = 0;
}
static __attribute__((optimize(0))) int _bug4_pattern(int part)
{
    int trigger = 0;
    _bug4_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug4_val += _bug4_arr[i];
        }
        __sync_synchronize();
        while (!_bug4_pred)
            pthread_yield();
        trigger = _bug4_val != 3;
        _bug4_pred = 0;
        _bug4_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug4_val += _bug4_arr[i];
        }
        __sync_synchronize();
        _bug4_pred = 1;
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
    static const uint32_t indices[] = {0x63};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x57ed7e47, 0x82cb10da};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xf3b53afe};
    static racebench_before_helper before_helpers[3] = {};
    _bug4_input_helper.interleave_num = 3;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xce6dca1f;
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
static volatile __attribute__((used)) int _bug5_pred;
static volatile __attribute__((used)) int _bug5_sent;
static void _bug5_pattern_clean(void)
{
    pthread_mutex_init(&_bug5_mutex1, NULL);
    pthread_cond_init(&_bug5_cond1, NULL);
    _bug5_pred = 0;
    _bug5_sent = 0;
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug5_mutex1);
        _bug5_pred = 1;
        pthread_cond_signal(&_bug5_cond1);
        _bug5_sent = 1;
        pthread_mutex_unlock(&_bug5_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug5_mutex1);
        if (_bug5_sent) {
            trigger = 1;
        }
        else {
            while (_bug5_pred == 0)
                pthread_cond_wait(&_bug5_cond1, &_bug5_mutex1);
        }
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
    static const uint32_t indices[] = {0x6b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x889fae6};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1201f5e};
    static racebench_before_helper before_helpers[2] = {};
    _bug5_input_helper.interleave_num = 2;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xa9aa1a44;
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
static volatile __attribute__((used)) int _bug6_released;
static void _bug6_pattern_clean(void)
{
    pthread_mutex_init(&_bug6_mutex1, NULL);
    _bug6_first_part = -1;
    _bug6_released = -1;
}
static __attribute__((optimize(0))) int _bug6_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug6_mutex1) == 0)
                break;
            if (_bug6_released == 0) {
                _bug6_first_part = -1;
                _bug6_released = -1;
                return 1;
            }
        }
        if (_bug6_first_part == -1)
            _bug6_first_part = part;
        else {
            _bug6_first_part = -1;
            while (_bug6_released == -1) {}
            _bug6_released = -1;
        }
        pthread_mutex_unlock(&_bug6_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug6_mutex1);
        if (_bug6_first_part == -1) {
            _bug6_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug6_mutex1);
                _bug6_released = 1;
            }
            else {
                _bug6_released = 0;
                while (_bug6_released != -1) {}
                pthread_mutex_unlock(&_bug6_mutex1);
                return 1;
            }
        }
        else {
            _bug6_first_part = -1;
            pthread_mutex_unlock(&_bug6_mutex1);
        }
    }
    return 0;
}

static void _bug6_pattern_clean(void);
static int _bug6_pattern(int part);
static racebench_bug_helper _bug6_bug_helper;
static racebench_input_helper _bug6_input_helper;
__attribute__((constructor))
static void _bug6_input_helper_init(void)
{
    static const uint32_t indices[] = {0x6f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x29201bb0, 0xfb83e3cf};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug6_input_helper.interleave_num = 3;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0xc5bc2bed;
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
static volatile __attribute__((used)) int* volatile _bug7_ptr;
static volatile __attribute__((used)) int _bug7_val;
static void _bug7_pattern_clean(void)
{
    pthread_mutex_init(&_bug7_mutex1, NULL);
    _bug7_ptr = NULL;
    _bug7_val = 0;
}
static __attribute__((optimize(0))) int _bug7_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug7_mutex1);
        _bug7_ptr = &_bug7_val;
        pthread_mutex_unlock(&_bug7_mutex1);
        pthread_mutex_lock(&_bug7_mutex1);
        if (_bug7_ptr == NULL)
            trigger = 1;
        _bug7_ptr = NULL;
        pthread_mutex_unlock(&_bug7_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug7_mutex1);
        _bug7_ptr = &_bug7_val;
        if (_bug7_ptr == NULL)
            trigger = 1;
        _bug7_ptr = NULL;
        pthread_mutex_unlock(&_bug7_mutex1);
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
    static const uint32_t indices[] = {0x7b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x4cc1e402};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182ffe};
    static racebench_before_helper before_helpers[2] = {};
    _bug7_input_helper.interleave_num = 2;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xedda1400;
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
    static const uint32_t indices[] = {0x1a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11b390e};
    static racebench_before_helper before_helpers[0] = {};
    _bug8_input_helper.interleave_num = 0;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0xa11b390e;
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
static volatile __attribute__((used)) int _bug9_val;
static void _bug9_pattern_clean(void)
{
    pthread_mutex_init(&_bug9_mutex1, NULL);
    _bug9_val = 0;
}
static __attribute__((optimize(0))) int _bug9_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug9_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug9_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug9_mutex1);
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

static void _bug9_pattern_clean(void);
static int _bug9_pattern(int part);
static racebench_bug_helper _bug9_bug_helper;
static racebench_input_helper _bug9_input_helper;
__attribute__((constructor))
static void _bug9_input_helper_init(void)
{
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa117ac7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug9_input_helper.interleave_num = 0;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0xa117ac7e;
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
    static const uint32_t indices[] = {0x1b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1163c8e};
    static racebench_before_helper before_helpers[0] = {};
    _bug10_input_helper.interleave_num = 0;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0xa1163c8e;
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
static volatile __attribute__((used)) int _bug11_pred;
static volatile __attribute__((used)) int _bug11_sent;
static void _bug11_pattern_clean(void)
{
    pthread_mutex_init(&_bug11_mutex1, NULL);
    pthread_cond_init(&_bug11_cond1, NULL);
    _bug11_pred = 0;
    _bug11_sent = 0;
}
static __attribute__((optimize(0))) int _bug11_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug11_mutex1);
        _bug11_pred = 1;
        pthread_cond_signal(&_bug11_cond1);
        _bug11_sent = 1;
        pthread_mutex_unlock(&_bug11_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug11_mutex1);
        if (_bug11_sent) {
            trigger = 1;
        }
        else {
            while (_bug11_pred == 0)
                pthread_cond_wait(&_bug11_cond1, &_bug11_mutex1);
        }
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
    static const uint32_t indices[] = {0xe};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x59258c4};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cde};
    static racebench_before_helper before_helpers[2] = {};
    _bug11_input_helper.interleave_num = 2;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0xa6aa85a2;
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
static volatile __attribute__((used)) int* volatile _bug12_ptr;
static volatile __attribute__((used)) int _bug12_val;
static void _bug12_pattern_clean(void)
{
    pthread_mutex_init(&_bug12_mutex1, NULL);
    _bug12_ptr = NULL;
    _bug12_val = 0;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug12_mutex1);
        _bug12_ptr = &_bug12_val;
        pthread_mutex_unlock(&_bug12_mutex1);
        pthread_mutex_lock(&_bug12_mutex1);
        if (_bug12_ptr == NULL)
            trigger = 1;
        _bug12_ptr = NULL;
        pthread_mutex_unlock(&_bug12_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug12_mutex1);
        _bug12_ptr = &_bug12_val;
        if (_bug12_ptr == NULL)
            trigger = 1;
        _bug12_ptr = NULL;
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
    static const uint32_t indices[] = {0x7f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x22e56884};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa8151d99};
    static racebench_before_helper before_helpers[2] = {};
    _bug12_input_helper.interleave_num = 2;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xcafa861d;
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
    static const uint32_t indices[] = {0x2d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xddad7d69, 0x26e44894};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115ad5e};
    static racebench_before_helper before_helpers[3] = {};
    _bug13_input_helper.interleave_num = 3;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0xa5a7735b;
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
static pthread_mutex_t _bug14_mutex2;
static void _bug14_pattern_clean(void)
{
    pthread_mutex_init(&_bug14_mutex1, NULL);
    pthread_mutex_init(&_bug14_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug14_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug14_mutex1);
        if (pthread_mutex_trylock(&_bug14_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug14_mutex2);
        pthread_mutex_unlock(&_bug14_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug14_mutex2);
        if (pthread_mutex_trylock(&_bug14_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug14_mutex1);
        pthread_mutex_unlock(&_bug14_mutex2);
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
    static const uint32_t indices[] = {0x33};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x5d7076dd, 0xefe98e9a};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182d2e};
    static racebench_before_helper before_helpers[3] = {};
    _bug14_input_helper.interleave_num = 3;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xee7232a5;
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
static volatile __attribute__((used)) int* volatile _bug15_ptr;
static volatile __attribute__((used)) int _bug15_val;
static void _bug15_pattern_clean(void)
{
    pthread_mutex_init(&_bug15_mutex1, NULL);
    _bug15_ptr = NULL;
    _bug15_val = 0;
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug15_mutex1);
        _bug15_ptr = &_bug15_val;
        pthread_mutex_unlock(&_bug15_mutex1);
        pthread_mutex_lock(&_bug15_mutex1);
        if (_bug15_ptr == NULL)
            trigger = 1;
        _bug15_ptr = NULL;
        pthread_mutex_unlock(&_bug15_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug15_mutex1);
        _bug15_ptr = &_bug15_val;
        if (_bug15_ptr == NULL)
            trigger = 1;
        _bug15_ptr = NULL;
        pthread_mutex_unlock(&_bug15_mutex1);
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
    static const uint32_t indices[] = {0x54};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb452c517, 0xbaff5beb};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug15_input_helper.interleave_num = 3;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0x106a4d80;
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
static volatile __attribute__((used)) int _bug16_ok;
static volatile __attribute__((used)) int* volatile _bug16_ptr;
static volatile __attribute__((used)) int _bug16_val;
static void _bug16_pattern_clean(void)
{
    _bug16_ok = 0;
    _bug16_ptr = NULL;
    _bug16_val = 0;
}
static __attribute__((optimize(0))) int _bug16_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug16_ok = 1;
        __sync_synchronize();
        _bug16_ptr = &_bug16_val;
    }
    else {
        while (!_bug16_ok)
            pthread_yield();
        if (_bug16_ptr == NULL)
            trigger = 1;
        _bug16_ptr = NULL;
        _bug16_ok = 0;
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
    static const uint32_t indices[] = {0x34};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb5abb98f, 0xf259854b};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182b8e};
    static racebench_before_helper before_helpers[3] = {};
    _bug16_input_helper.interleave_num = 3;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0x491d6a68;
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
static volatile __attribute__((used)) int _bug17_ok;
static volatile __attribute__((used)) int* volatile _bug17_ptr;
static volatile __attribute__((used)) int _bug17_val;
static void _bug17_pattern_clean(void)
{
    _bug17_ok = 0;
    _bug17_ptr = NULL;
    _bug17_val = 0;
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug17_ok = 1;
        __sync_synchronize();
        _bug17_ptr = &_bug17_val;
    }
    else {
        while (!_bug17_ok)
            pthread_yield();
        if (_bug17_ptr == NULL)
            trigger = 1;
        _bug17_ptr = NULL;
        _bug17_ok = 0;
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
    static const uint32_t indices[] = {0x15};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x51c2ea84, 0xf7f62e82};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug17_input_helper.interleave_num = 3;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0xead145c4;
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
    static const uint32_t indices[] = {0x45};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x77550c82};
    static racebench_before_helper before_helpers[0] = {};
    _bug18_input_helper.interleave_num = 0;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0x77550c82;
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
    static const uint32_t indices[] = {0x38};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x14854c99};
    static racebench_before_helper before_helpers[1] = {};
    _bug19_input_helper.interleave_num = 1;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0x14854c99;
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
static volatile __attribute__((used)) int _bug20_first_part;
static volatile __attribute__((used)) int _bug20_released;
static void _bug20_pattern_clean(void)
{
    pthread_mutex_init(&_bug20_mutex1, NULL);
    _bug20_first_part = -1;
    _bug20_released = -1;
}
static __attribute__((optimize(0))) int _bug20_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug20_mutex1) == 0)
                break;
            if (_bug20_released == 0) {
                _bug20_first_part = -1;
                _bug20_released = -1;
                return 1;
            }
        }
        if (_bug20_first_part == -1)
            _bug20_first_part = part;
        else {
            _bug20_first_part = -1;
            while (_bug20_released == -1) {}
            _bug20_released = -1;
        }
        pthread_mutex_unlock(&_bug20_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug20_mutex1);
        if (_bug20_first_part == -1) {
            _bug20_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug20_mutex1);
                _bug20_released = 1;
            }
            else {
                _bug20_released = 0;
                while (_bug20_released != -1) {}
                pthread_mutex_unlock(&_bug20_mutex1);
                return 1;
            }
        }
        else {
            _bug20_first_part = -1;
            pthread_mutex_unlock(&_bug20_mutex1);
        }
    }
    return 0;
}

static void _bug20_pattern_clean(void);
static int _bug20_pattern(int part);
static racebench_bug_helper _bug20_bug_helper;
static racebench_input_helper _bug20_input_helper;
__attribute__((constructor))
static void _bug20_input_helper_init(void)
{
    static const uint32_t indices[] = {0xb};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x1b923664};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa118255e};
    static racebench_before_helper before_helpers[2] = {};
    _bug20_input_helper.interleave_num = 2;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xbcaa5bc2;
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
    static const uint32_t indices[] = {0x15};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb7cf4b36};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182d5e};
    static racebench_before_helper before_helpers[2] = {};
    _bug21_input_helper.interleave_num = 2;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0x58e77894;
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
static volatile __attribute__((used)) int _bug22_first_part;
static volatile __attribute__((used)) int _bug22_released;
static void _bug22_pattern_clean(void)
{
    pthread_mutex_init(&_bug22_mutex1, NULL);
    _bug22_first_part = -1;
    _bug22_released = -1;
}
static __attribute__((optimize(0))) int _bug22_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug22_mutex1) == 0)
                break;
            if (_bug22_released == 0) {
                _bug22_first_part = -1;
                _bug22_released = -1;
                return 1;
            }
        }
        if (_bug22_first_part == -1)
            _bug22_first_part = part;
        else {
            _bug22_first_part = -1;
            while (_bug22_released == -1) {}
            _bug22_released = -1;
        }
        pthread_mutex_unlock(&_bug22_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug22_mutex1);
        if (_bug22_first_part == -1) {
            _bug22_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug22_mutex1);
                _bug22_released = 1;
            }
            else {
                _bug22_released = 0;
                while (_bug22_released != -1) {}
                pthread_mutex_unlock(&_bug22_mutex1);
                return 1;
            }
        }
        else {
            _bug22_first_part = -1;
            pthread_mutex_unlock(&_bug22_mutex1);
        }
    }
    return 0;
}

static void _bug22_pattern_clean(void);
static int _bug22_pattern(int part);
static racebench_bug_helper _bug22_bug_helper;
static racebench_input_helper _bug22_input_helper;
__attribute__((constructor))
static void _bug22_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug22_input_helper.interleave_num = 0;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0xa1182e1e;
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
static volatile __attribute__((used)) int _bug23_released;
static void _bug23_pattern_clean(void)
{
    pthread_mutex_init(&_bug23_mutex1, NULL);
    _bug23_first_part = -1;
    _bug23_released = -1;
}
static __attribute__((optimize(0))) int _bug23_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug23_mutex1) == 0)
                break;
            if (_bug23_released == 0) {
                _bug23_first_part = -1;
                _bug23_released = -1;
                return 1;
            }
        }
        if (_bug23_first_part == -1)
            _bug23_first_part = part;
        else {
            _bug23_first_part = -1;
            while (_bug23_released == -1) {}
            _bug23_released = -1;
        }
        pthread_mutex_unlock(&_bug23_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug23_mutex1);
        if (_bug23_first_part == -1) {
            _bug23_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug23_mutex1);
                _bug23_released = 1;
            }
            else {
                _bug23_released = 0;
                while (_bug23_released != -1) {}
                pthread_mutex_unlock(&_bug23_mutex1);
                return 1;
            }
        }
        else {
            _bug23_first_part = -1;
            pthread_mutex_unlock(&_bug23_mutex1);
        }
    }
    return 0;
}

static void _bug23_pattern_clean(void);
static int _bug23_pattern(int part);
static racebench_bug_helper _bug23_bug_helper;
static racebench_input_helper _bug23_input_helper;
__attribute__((constructor))
static void _bug23_input_helper_init(void)
{
    static const uint32_t indices[] = {0x41};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x1026fe2, 0xedaff089};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1121c4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug23_input_helper.interleave_num = 3;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0x8fc47cb9;
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
    static const uint32_t indices[] = {0xa};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x515c1f41, 0x4d7dce1e};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e8e};
    static racebench_before_helper before_helpers[3] = {};
    _bug24_input_helper.interleave_num = 3;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0x3ff21bed;
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
static volatile __attribute__((used)) int _bug25_arr[3];
static volatile __attribute__((used)) int _bug25_val;
static volatile __attribute__((used)) int _bug25_pred;
static void _bug25_pattern_clean(void)
{
    _bug25_arr[0] = 0, _bug25_arr[1] = 2, _bug25_arr[2] = 1;
    _bug25_val = 0;
    _bug25_pred = 0;
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    _bug25_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug25_val += _bug25_arr[i];
        }
        __sync_synchronize();
        while (!_bug25_pred)
            pthread_yield();
        trigger = _bug25_val != 3;
        _bug25_pred = 0;
        _bug25_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug25_val += _bug25_arr[i];
        }
        __sync_synchronize();
        _bug25_pred = 1;
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
    static const uint32_t indices[] = {0x45};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf4252e2f};
    static racebench_before_helper before_helpers[0] = {};
    _bug25_input_helper.interleave_num = 0;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xf4252e2f;
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
static pthread_cond_t _bug26_cond1;
static volatile __attribute__((used)) int _bug26_pred;
static volatile __attribute__((used)) int _bug26_sent;
static void _bug26_pattern_clean(void)
{
    pthread_mutex_init(&_bug26_mutex1, NULL);
    pthread_cond_init(&_bug26_cond1, NULL);
    _bug26_pred = 0;
    _bug26_sent = 0;
}
static __attribute__((optimize(0))) int _bug26_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug26_mutex1);
        _bug26_pred = 1;
        pthread_cond_signal(&_bug26_cond1);
        _bug26_sent = 1;
        pthread_mutex_unlock(&_bug26_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug26_mutex1);
        if (_bug26_sent) {
            trigger = 1;
        }
        else {
            while (_bug26_pred == 0)
                pthread_cond_wait(&_bug26_cond1, &_bug26_mutex1);
        }
        _bug26_pred = 0;
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
    static const uint32_t indices[] = {0x41};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xd0f5185f};
    static racebench_before_helper before_helpers[0] = {};
    _bug26_input_helper.interleave_num = 0;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0xd0f5185f;
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
    static const uint32_t indices[] = {0x4b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x74820c12, 0xadbd1c37};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa117879e};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0xc356afe7;
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
    static const uint32_t indices[] = {0x4};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7f02d418};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1146c6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug28_input_helper.interleave_num = 2;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0x20174086;
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
static volatile __attribute__((used)) int _bug29_first_part;
static volatile __attribute__((used)) int _bug29_released;
static void _bug29_pattern_clean(void)
{
    pthread_mutex_init(&_bug29_mutex1, NULL);
    _bug29_first_part = -1;
    _bug29_released = -1;
}
static __attribute__((optimize(0))) int _bug29_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug29_mutex1) == 0)
                break;
            if (_bug29_released == 0) {
                _bug29_first_part = -1;
                _bug29_released = -1;
                return 1;
            }
        }
        if (_bug29_first_part == -1)
            _bug29_first_part = part;
        else {
            _bug29_first_part = -1;
            while (_bug29_released == -1) {}
            _bug29_released = -1;
        }
        pthread_mutex_unlock(&_bug29_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug29_mutex1);
        if (_bug29_first_part == -1) {
            _bug29_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug29_mutex1);
                _bug29_released = 1;
            }
            else {
                _bug29_released = 0;
                while (_bug29_released != -1) {}
                pthread_mutex_unlock(&_bug29_mutex1);
                return 1;
            }
        }
        else {
            _bug29_first_part = -1;
            pthread_mutex_unlock(&_bug29_mutex1);
        }
    }
    return 0;
}

static void _bug29_pattern_clean(void);
static int _bug29_pattern(int part);
static racebench_bug_helper _bug29_bug_helper;
static racebench_input_helper _bug29_input_helper;
__attribute__((constructor))
static void _bug29_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x89ba1b8c};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11830fe};
    static racebench_before_helper before_helpers[2] = {};
    _bug29_input_helper.interleave_num = 2;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x2ad24c8a;
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
static pthread_mutex_t _bug30_mutex2;
static void _bug30_pattern_clean(void)
{
    pthread_mutex_init(&_bug30_mutex1, NULL);
    pthread_mutex_init(&_bug30_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug30_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug30_mutex1);
        if (pthread_mutex_trylock(&_bug30_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug30_mutex2);
        pthread_mutex_unlock(&_bug30_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug30_mutex2);
        if (pthread_mutex_trylock(&_bug30_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug30_mutex1);
        pthread_mutex_unlock(&_bug30_mutex2);
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
    static const uint32_t indices[] = {0x3b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa6e30e5a};
    static racebench_before_helper before_helpers[1] = {};
    _bug30_input_helper.interleave_num = 1;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0xa6e30e5a;
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
static volatile __attribute__((used)) int* volatile _bug31_ptr;
static volatile __attribute__((used)) int _bug31_val;
static void _bug31_pattern_clean(void)
{
    pthread_mutex_init(&_bug31_mutex1, NULL);
    _bug31_ptr = NULL;
    _bug31_val = 0;
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug31_mutex1);
        _bug31_ptr = &_bug31_val;
        pthread_mutex_unlock(&_bug31_mutex1);
        pthread_mutex_lock(&_bug31_mutex1);
        if (_bug31_ptr == NULL)
            trigger = 1;
        _bug31_ptr = NULL;
        pthread_mutex_unlock(&_bug31_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug31_mutex1);
        _bug31_ptr = &_bug31_val;
        if (_bug31_ptr == NULL)
            trigger = 1;
        _bug31_ptr = NULL;
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
    static const uint32_t indices[] = {0x6a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x45ecdb3d, 0x4eb455b5};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa112ac6e};
    static racebench_before_helper before_helpers[3] = {};
    _bug31_input_helper.interleave_num = 3;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0x35b3dd60;
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
    static const uint32_t indices[] = {0x79};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa118263e};
    static racebench_before_helper before_helpers[1] = {};
    _bug32_input_helper.interleave_num = 1;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0xa118263e;
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
static volatile __attribute__((used)) int _bug33_val;
static void _bug33_pattern_clean(void)
{
    pthread_mutex_init(&_bug33_mutex1, NULL);
    _bug33_val = 0;
}
static __attribute__((optimize(0))) int _bug33_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug33_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug33_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug33_mutex1);
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

static void _bug33_pattern_clean(void);
static int _bug33_pattern(int part);
static racebench_bug_helper _bug33_bug_helper;
static racebench_input_helper _bug33_input_helper;
__attribute__((constructor))
static void _bug33_input_helper_init(void)
{
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug33_input_helper.interleave_num = 1;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0xa1182c4e;
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
static volatile __attribute__((used)) int _bug34_released;
static void _bug34_pattern_clean(void)
{
    pthread_mutex_init(&_bug34_mutex1, NULL);
    _bug34_first_part = -1;
    _bug34_released = -1;
}
static __attribute__((optimize(0))) int _bug34_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug34_mutex1) == 0)
                break;
            if (_bug34_released == 0) {
                _bug34_first_part = -1;
                _bug34_released = -1;
                return 1;
            }
        }
        if (_bug34_first_part == -1)
            _bug34_first_part = part;
        else {
            _bug34_first_part = -1;
            while (_bug34_released == -1) {}
            _bug34_released = -1;
        }
        pthread_mutex_unlock(&_bug34_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug34_mutex1);
        if (_bug34_first_part == -1) {
            _bug34_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug34_mutex1);
                _bug34_released = 1;
            }
            else {
                _bug34_released = 0;
                while (_bug34_released != -1) {}
                pthread_mutex_unlock(&_bug34_mutex1);
                return 1;
            }
        }
        else {
            _bug34_first_part = -1;
            pthread_mutex_unlock(&_bug34_mutex1);
        }
    }
    return 0;
}

static void _bug34_pattern_clean(void);
static int _bug34_pattern(int part);
static racebench_bug_helper _bug34_bug_helper;
static racebench_input_helper _bug34_input_helper;
__attribute__((constructor))
static void _bug34_input_helper_init(void)
{
    static const uint32_t indices[] = {0x76};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x63b01349, 0x8b02b1fe};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1156cbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug34_input_helper.interleave_num = 3;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0x8fc83205;
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
    static const uint32_t indices[] = {0x48};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xfdbdf68e, 0x3655f6d1};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa796b11c};
    static racebench_before_helper before_helpers[3] = {};
    _bug35_input_helper.interleave_num = 3;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xdbaa9e7b;
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
    static const uint32_t indices[] = {0x25};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x88c6e1de, 0x545c4ac3};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa117954e};
    static racebench_before_helper before_helpers[3] = {};
    _bug36_input_helper.interleave_num = 3;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0x7e3ac1ef;
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
    static const uint32_t indices[] = {0x1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug37_input_helper.interleave_num = 0;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int _bug38_val;
static void _bug38_pattern_clean(void)
{
    pthread_mutex_init(&_bug38_mutex1, NULL);
    _bug38_val = 0;
}
static __attribute__((optimize(0))) int _bug38_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug38_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug38_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug38_mutex1);
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

static void _bug38_pattern_clean(void);
static int _bug38_pattern(int part);
static racebench_bug_helper _bug38_bug_helper;
static racebench_input_helper _bug38_input_helper;
__attribute__((constructor))
static void _bug38_input_helper_init(void)
{
    static const uint32_t indices[] = {0x4};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x7780e4b6, 0x5083aa80};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x363ec90};
    static racebench_before_helper before_helpers[3] = {};
    _bug38_input_helper.interleave_num = 3;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xcb687bc6;
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
static pthread_cond_t _bug39_cond1;
static volatile __attribute__((used)) int _bug39_pred;
static volatile __attribute__((used)) int _bug39_sent;
static void _bug39_pattern_clean(void)
{
    pthread_mutex_init(&_bug39_mutex1, NULL);
    pthread_cond_init(&_bug39_cond1, NULL);
    _bug39_pred = 0;
    _bug39_sent = 0;
}
static __attribute__((optimize(0))) int _bug39_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug39_mutex1);
        _bug39_pred = 1;
        pthread_cond_signal(&_bug39_cond1);
        _bug39_sent = 1;
        pthread_mutex_unlock(&_bug39_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug39_mutex1);
        if (_bug39_sent) {
            trigger = 1;
        }
        else {
            while (_bug39_pred == 0)
                pthread_cond_wait(&_bug39_cond1, &_bug39_mutex1);
        }
        _bug39_pred = 0;
        pthread_mutex_unlock(&_bug39_mutex1);
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
    static const uint32_t indices[] = {0x75};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x5282aaf, 0xedc1e8d5};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11b2f5e};
    static racebench_before_helper before_helpers[3] = {};
    _bug39_input_helper.interleave_num = 3;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0x940542e2;
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
static volatile __attribute__((used)) int _bug40_arr[3];
static volatile __attribute__((used)) int _bug40_val;
static volatile __attribute__((used)) int _bug40_pred;
static void _bug40_pattern_clean(void)
{
    _bug40_arr[0] = 0, _bug40_arr[1] = 2, _bug40_arr[2] = 1;
    _bug40_val = 0;
    _bug40_pred = 0;
}
static __attribute__((optimize(0))) int _bug40_pattern(int part)
{
    int trigger = 0;
    _bug40_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug40_val += _bug40_arr[i];
        }
        __sync_synchronize();
        while (!_bug40_pred)
            pthread_yield();
        trigger = _bug40_val != 3;
        _bug40_pred = 0;
        _bug40_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug40_val += _bug40_arr[i];
        }
        __sync_synchronize();
        _bug40_pred = 1;
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
    static const uint32_t indices[] = {0x6a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb005a8f1, 0xd7f23528};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xe406faca};
    static racebench_before_helper before_helpers[3] = {};
    _bug40_input_helper.interleave_num = 3;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0x6bfed8e3;
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
static pthread_mutex_t _bug41_mutex2;
static void _bug41_pattern_clean(void)
{
    pthread_mutex_init(&_bug41_mutex1, NULL);
    pthread_mutex_init(&_bug41_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug41_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug41_mutex1);
        if (pthread_mutex_trylock(&_bug41_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug41_mutex2);
        pthread_mutex_unlock(&_bug41_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug41_mutex2);
        if (pthread_mutex_trylock(&_bug41_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug41_mutex1);
        pthread_mutex_unlock(&_bug41_mutex2);
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
    static const uint32_t indices[] = {0x40};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d3e};
    static racebench_before_helper before_helpers[1] = {};
    _bug41_input_helper.interleave_num = 1;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa1182d3e;
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
    static const uint32_t indices[] = {0x71};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x43d43897, 0xe8157999};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa118245e};
    static racebench_before_helper before_helpers[3] = {};
    _bug42_input_helper.interleave_num = 3;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0xcd01d68e;
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
static volatile __attribute__((used)) int* volatile _bug43_ptr;
static volatile __attribute__((used)) int _bug43_val;
static void _bug43_pattern_clean(void)
{
    pthread_mutex_init(&_bug43_mutex1, NULL);
    _bug43_ptr = NULL;
    _bug43_val = 0;
}
static __attribute__((optimize(0))) int _bug43_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug43_mutex1);
        _bug43_ptr = &_bug43_val;
        pthread_mutex_unlock(&_bug43_mutex1);
        pthread_mutex_lock(&_bug43_mutex1);
        if (_bug43_ptr == NULL)
            trigger = 1;
        _bug43_ptr = NULL;
        pthread_mutex_unlock(&_bug43_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug43_mutex1);
        _bug43_ptr = &_bug43_val;
        if (_bug43_ptr == NULL)
            trigger = 1;
        _bug43_ptr = NULL;
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
    static const uint32_t indices[] = {0x29};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x12fc9ea5, 0x5be32116};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1146cde};
    static racebench_before_helper before_helpers[3] = {};
    _bug43_input_helper.interleave_num = 3;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0xff42c99;
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
static volatile __attribute__((used)) int _bug44_val;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    _bug44_val = 0;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug44_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug44_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug44_mutex1);
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

static void _bug44_pattern_clean(void);
static int _bug44_pattern(int part);
static racebench_bug_helper _bug44_bug_helper;
static racebench_input_helper _bug44_input_helper;
__attribute__((constructor))
static void _bug44_input_helper_init(void)
{
    static const uint32_t indices[] = {0x63};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115cc7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug44_input_helper.interleave_num = 0;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xa115cc7e;
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
static pthread_mutex_t _bug45_mutex2;
static void _bug45_pattern_clean(void)
{
    pthread_mutex_init(&_bug45_mutex1, NULL);
    pthread_mutex_init(&_bug45_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug45_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug45_mutex1);
        if (pthread_mutex_trylock(&_bug45_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug45_mutex2);
        pthread_mutex_unlock(&_bug45_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug45_mutex2);
        if (pthread_mutex_trylock(&_bug45_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug45_mutex1);
        pthread_mutex_unlock(&_bug45_mutex2);
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
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xc5343d90};
    static racebench_before_helper before_helpers[1] = {};
    _bug45_input_helper.interleave_num = 1;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0xc5343d90;
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
static volatile __attribute__((used)) int _bug46_arr[3];
static volatile __attribute__((used)) int _bug46_val;
static volatile __attribute__((used)) int _bug46_pred;
static void _bug46_pattern_clean(void)
{
    _bug46_arr[0] = 0, _bug46_arr[1] = 2, _bug46_arr[2] = 1;
    _bug46_val = 0;
    _bug46_pred = 0;
}
static __attribute__((optimize(0))) int _bug46_pattern(int part)
{
    int trigger = 0;
    _bug46_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug46_val += _bug46_arr[i];
        }
        __sync_synchronize();
        while (!_bug46_pred)
            pthread_yield();
        trigger = _bug46_val != 3;
        _bug46_pred = 0;
        _bug46_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug46_val += _bug46_arr[i];
        }
        __sync_synchronize();
        _bug46_pred = 1;
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
    static const uint32_t indices[] = {0x51};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xd84a2722};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118308e};
    static racebench_before_helper before_helpers[2] = {};
    _bug46_input_helper.interleave_num = 2;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0x796257b0;
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
static volatile __attribute__((used)) int _bug47_first_part;
static void _bug47_pattern_clean(void)
{
    pthread_mutex_init(&_bug47_mutex1, NULL);
    _bug47_first_part = -1;
}
static __attribute__((optimize(0))) int _bug47_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug47_mutex1);
        if (_bug47_first_part == -1)
            _bug47_first_part = part;
        else
            _bug47_first_part = -1;
        pthread_mutex_unlock(&_bug47_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug47_mutex1);
        if (_bug47_first_part == -1) {
            _bug47_first_part = part;
            trigger = 1;
        }
        else
            _bug47_first_part = -1;
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
    static const uint32_t indices[] = {0x64};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x5eb676e7, 0x38c527b2};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xc3a7303c};
    static racebench_before_helper before_helpers[3] = {};
    _bug47_input_helper.interleave_num = 3;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0x5b22ced5;
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
static void _bug48_pattern_clean(void)
{
    pthread_mutex_init(&_bug48_mutex1, NULL);
    _bug48_first_part = -1;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug48_mutex1);
        if (_bug48_first_part == -1)
            _bug48_first_part = part;
        else
            _bug48_first_part = -1;
        pthread_mutex_unlock(&_bug48_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug48_mutex1);
        if (_bug48_first_part == -1) {
            _bug48_first_part = part;
            trigger = 1;
        }
        else
            _bug48_first_part = -1;
        pthread_mutex_unlock(&_bug48_mutex1);
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
    static const uint32_t indices[] = {0x5a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa11e723e};
    static racebench_before_helper before_helpers[1] = {};
    _bug48_input_helper.interleave_num = 1;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0xa11e723e;
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
    static const uint32_t indices[] = {0x64};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug49_input_helper.interleave_num = 0;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xa1182c6e;
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
static volatile __attribute__((used)) int* volatile _bug50_ptr;
static volatile __attribute__((used)) int _bug50_val;
static volatile __attribute__((used)) int _bug50_pred;
static void _bug50_pattern_clean(void)
{
    pthread_mutex_init(&_bug50_mutex1, NULL);
    pthread_cond_init(&_bug50_cond1, NULL);
    _bug50_ptr = NULL;
    _bug50_val = 0;
    _bug50_pred = 0;
}
static __attribute__((optimize(0))) int _bug50_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug50_pred = 0;
        pthread_cond_signal(&_bug50_cond1);
        pthread_mutex_lock(&_bug50_mutex1);
        _bug50_ptr = &_bug50_val;
        _bug50_pred = 1;
        pthread_mutex_unlock(&_bug50_mutex1);
        pthread_cond_signal(&_bug50_cond1);
    }
    else {
        pthread_mutex_lock(&_bug50_mutex1);
        if (!_bug50_pred)
            pthread_cond_wait(&_bug50_cond1, &_bug50_mutex1);
        if (_bug50_ptr == NULL)
            trigger = 1;
        _bug50_ptr = NULL;
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
    static const uint32_t indices[] = {0x17};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xe1099551};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x33522ae9};
    static racebench_before_helper before_helpers[2] = {};
    _bug50_input_helper.interleave_num = 2;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0x145bc03a;
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
    static const uint32_t indices[] = {0x63};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115795e};
    static racebench_before_helper before_helpers[0] = {};
    _bug51_input_helper.interleave_num = 0;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0xa115795e;
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
static volatile __attribute__((used)) int _bug52_arr[3];
static volatile __attribute__((used)) int _bug52_val;
static volatile __attribute__((used)) int _bug52_pred;
static void _bug52_pattern_clean(void)
{
    _bug52_arr[0] = 0, _bug52_arr[1] = 2, _bug52_arr[2] = 1;
    _bug52_val = 0;
    _bug52_pred = 0;
}
static __attribute__((optimize(0))) int _bug52_pattern(int part)
{
    int trigger = 0;
    _bug52_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug52_val += _bug52_arr[i];
        }
        __sync_synchronize();
        while (!_bug52_pred)
            pthread_yield();
        trigger = _bug52_val != 3;
        _bug52_pred = 0;
        _bug52_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug52_val += _bug52_arr[i];
        }
        __sync_synchronize();
        _bug52_pred = 1;
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
    static const uint32_t indices[] = {0x27};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x8b9e5e54};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x26c537f8};
    static racebench_before_helper before_helpers[2] = {};
    _bug52_input_helper.interleave_num = 2;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0xb263964c;
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
    static const uint32_t indices[] = {0x74};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1149ace};
    static racebench_before_helper before_helpers[0] = {};
    _bug53_input_helper.interleave_num = 0;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0xa1149ace;
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
static volatile __attribute__((used)) int _bug54_arr[3];
static volatile __attribute__((used)) int _bug54_val;
static volatile __attribute__((used)) int _bug54_pred;
static void _bug54_pattern_clean(void)
{
    _bug54_arr[0] = 0, _bug54_arr[1] = 2, _bug54_arr[2] = 1;
    _bug54_val = 0;
    _bug54_pred = 0;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    _bug54_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug54_val += _bug54_arr[i];
        }
        __sync_synchronize();
        while (!_bug54_pred)
            pthread_yield();
        trigger = _bug54_val != 3;
        _bug54_pred = 0;
        _bug54_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug54_val += _bug54_arr[i];
        }
        __sync_synchronize();
        _bug54_pred = 1;
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
    static const uint32_t indices[] = {0x3f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x3889eac8, 0x473d6b17};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x7475ad49};
    static racebench_before_helper before_helpers[3] = {};
    _bug54_input_helper.interleave_num = 3;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0xf43d0328;
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
static volatile __attribute__((used)) int* volatile _bug55_ptr;
static volatile __attribute__((used)) int _bug55_val;
static volatile __attribute__((used)) int _bug55_pred;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    pthread_cond_init(&_bug55_cond1, NULL);
    _bug55_ptr = NULL;
    _bug55_val = 0;
    _bug55_pred = 0;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug55_pred = 0;
        pthread_cond_signal(&_bug55_cond1);
        pthread_mutex_lock(&_bug55_mutex1);
        _bug55_ptr = &_bug55_val;
        _bug55_pred = 1;
        pthread_mutex_unlock(&_bug55_mutex1);
        pthread_cond_signal(&_bug55_cond1);
    }
    else {
        pthread_mutex_lock(&_bug55_mutex1);
        if (!_bug55_pred)
            pthread_cond_wait(&_bug55_cond1, &_bug55_mutex1);
        if (_bug55_ptr == NULL)
            trigger = 1;
        _bug55_ptr = NULL;
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
    static const uint32_t indices[] = {0xa};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdf47253f, 0x7b624a9c};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11dc0ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0xfbc73089;
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
static volatile __attribute__((used)) int _bug56_pred;
static volatile __attribute__((used)) int _bug56_sent;
static void _bug56_pattern_clean(void)
{
    pthread_mutex_init(&_bug56_mutex1, NULL);
    pthread_cond_init(&_bug56_cond1, NULL);
    _bug56_pred = 0;
    _bug56_sent = 0;
}
static __attribute__((optimize(0))) int _bug56_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug56_mutex1);
        _bug56_pred = 1;
        pthread_cond_signal(&_bug56_cond1);
        _bug56_sent = 1;
        pthread_mutex_unlock(&_bug56_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug56_mutex1);
        if (_bug56_sent) {
            trigger = 1;
        }
        else {
            while (_bug56_pred == 0)
                pthread_cond_wait(&_bug56_cond1, &_bug56_mutex1);
        }
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
    static const uint32_t indices[] = {0x76};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[0] = {};
    _bug56_input_helper.interleave_num = 0;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0xa1182cbe;
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
    static const uint32_t indices[] = {0x53};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x851108c0};
    static racebench_before_helper before_helpers[1] = {};
    _bug57_input_helper.interleave_num = 1;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0x851108c0;
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
    static const uint32_t indices[] = {0x82};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x721d4e89};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xf8f28fd5};
    static racebench_before_helper before_helpers[2] = {};
    _bug58_input_helper.interleave_num = 2;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0x6b0fde5e;
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
static volatile __attribute__((used)) int _bug59_arr[3];
static volatile __attribute__((used)) int _bug59_val;
static volatile __attribute__((used)) int _bug59_pred;
static void _bug59_pattern_clean(void)
{
    _bug59_arr[0] = 0, _bug59_arr[1] = 2, _bug59_arr[2] = 1;
    _bug59_val = 0;
    _bug59_pred = 0;
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    int trigger = 0;
    _bug59_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug59_val += _bug59_arr[i];
        }
        __sync_synchronize();
        while (!_bug59_pred)
            pthread_yield();
        trigger = _bug59_val != 3;
        _bug59_pred = 0;
        _bug59_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug59_val += _bug59_arr[i];
        }
        __sync_synchronize();
        _bug59_pred = 1;
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
    static const uint32_t indices[] = {0x39};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xb2ed1642};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115bb2e};
    static racebench_before_helper before_helpers[2] = {};
    _bug59_input_helper.interleave_num = 2;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0x5402d170;
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
static volatile __attribute__((used)) int _bug60_first_part;
static volatile __attribute__((used)) int _bug60_released;
static void _bug60_pattern_clean(void)
{
    pthread_mutex_init(&_bug60_mutex1, NULL);
    _bug60_first_part = -1;
    _bug60_released = -1;
}
static __attribute__((optimize(0))) int _bug60_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug60_mutex1) == 0)
                break;
            if (_bug60_released == 0) {
                _bug60_first_part = -1;
                _bug60_released = -1;
                return 1;
            }
        }
        if (_bug60_first_part == -1)
            _bug60_first_part = part;
        else {
            _bug60_first_part = -1;
            while (_bug60_released == -1) {}
            _bug60_released = -1;
        }
        pthread_mutex_unlock(&_bug60_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug60_mutex1);
        if (_bug60_first_part == -1) {
            _bug60_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug60_mutex1);
                _bug60_released = 1;
            }
            else {
                _bug60_released = 0;
                while (_bug60_released != -1) {}
                pthread_mutex_unlock(&_bug60_mutex1);
                return 1;
            }
        }
        else {
            _bug60_first_part = -1;
            pthread_mutex_unlock(&_bug60_mutex1);
        }
    }
    return 0;
}

static void _bug60_pattern_clean(void);
static int _bug60_pattern(int part);
static racebench_bug_helper _bug60_bug_helper;
static racebench_input_helper _bug60_input_helper;
__attribute__((constructor))
static void _bug60_input_helper_init(void)
{
    static const uint32_t indices[] = {0x24};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x33d7f23};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182dee};
    static racebench_before_helper before_helpers[2] = {};
    _bug60_input_helper.interleave_num = 2;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0xa455ad11;
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
static volatile __attribute__((used)) int* volatile _bug61_ptr;
static volatile __attribute__((used)) int _bug61_val;
static volatile __attribute__((used)) int _bug61_pred;
static void _bug61_pattern_clean(void)
{
    pthread_mutex_init(&_bug61_mutex1, NULL);
    pthread_cond_init(&_bug61_cond1, NULL);
    _bug61_ptr = NULL;
    _bug61_val = 0;
    _bug61_pred = 0;
}
static __attribute__((optimize(0))) int _bug61_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug61_pred = 0;
        pthread_cond_signal(&_bug61_cond1);
        pthread_mutex_lock(&_bug61_mutex1);
        _bug61_ptr = &_bug61_val;
        _bug61_pred = 1;
        pthread_mutex_unlock(&_bug61_mutex1);
        pthread_cond_signal(&_bug61_cond1);
    }
    else {
        pthread_mutex_lock(&_bug61_mutex1);
        if (!_bug61_pred)
            pthread_cond_wait(&_bug61_cond1, &_bug61_mutex1);
        if (_bug61_ptr == NULL)
            trigger = 1;
        _bug61_ptr = NULL;
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
    static const uint32_t indices[] = {0x4b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xe5534dd6};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182b4e};
    static racebench_before_helper before_helpers[2] = {};
    _bug61_input_helper.interleave_num = 2;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0x866b7924;
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
static volatile __attribute__((used)) int _bug62_arr[3];
static volatile __attribute__((used)) int _bug62_val;
static volatile __attribute__((used)) int _bug62_pred;
static void _bug62_pattern_clean(void)
{
    _bug62_arr[0] = 0, _bug62_arr[1] = 2, _bug62_arr[2] = 1;
    _bug62_val = 0;
    _bug62_pred = 0;
}
static __attribute__((optimize(0))) int _bug62_pattern(int part)
{
    int trigger = 0;
    _bug62_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug62_val += _bug62_arr[i];
        }
        __sync_synchronize();
        while (!_bug62_pred)
            pthread_yield();
        trigger = _bug62_val != 3;
        _bug62_pred = 0;
        _bug62_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug62_val += _bug62_arr[i];
        }
        __sync_synchronize();
        _bug62_pred = 1;
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
    static const uint32_t indices[] = {0x75};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x6f54c8b};
    static racebench_before_helper before_helpers[0] = {};
    _bug62_input_helper.interleave_num = 0;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0x6f54c8b;
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
static volatile __attribute__((used)) int _bug63_released;
static void _bug63_pattern_clean(void)
{
    pthread_mutex_init(&_bug63_mutex1, NULL);
    _bug63_first_part = -1;
    _bug63_released = -1;
}
static __attribute__((optimize(0))) int _bug63_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug63_mutex1) == 0)
                break;
            if (_bug63_released == 0) {
                _bug63_first_part = -1;
                _bug63_released = -1;
                return 1;
            }
        }
        if (_bug63_first_part == -1)
            _bug63_first_part = part;
        else {
            _bug63_first_part = -1;
            while (_bug63_released == -1) {}
            _bug63_released = -1;
        }
        pthread_mutex_unlock(&_bug63_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug63_mutex1);
        if (_bug63_first_part == -1) {
            _bug63_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug63_mutex1);
                _bug63_released = 1;
            }
            else {
                _bug63_released = 0;
                while (_bug63_released != -1) {}
                pthread_mutex_unlock(&_bug63_mutex1);
                return 1;
            }
        }
        else {
            _bug63_first_part = -1;
            pthread_mutex_unlock(&_bug63_mutex1);
        }
    }
    return 0;
}

static void _bug63_pattern_clean(void);
static int _bug63_pattern(int part);
static racebench_bug_helper _bug63_bug_helper;
static racebench_input_helper _bug63_input_helper;
__attribute__((constructor))
static void _bug63_input_helper_init(void)
{
    static const uint32_t indices[] = {0x36};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x669413cb, 0xae1c595};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xe4a51d9f};
    static racebench_before_helper before_helpers[3] = {};
    _bug63_input_helper.interleave_num = 3;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x561af6ff;
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
static pthread_mutex_t _bug64_mutex2;
static void _bug64_pattern_clean(void)
{
    pthread_mutex_init(&_bug64_mutex1, NULL);
    pthread_mutex_init(&_bug64_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug64_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug64_mutex1);
        if (pthread_mutex_trylock(&_bug64_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug64_mutex2);
        pthread_mutex_unlock(&_bug64_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug64_mutex2);
        if (pthread_mutex_trylock(&_bug64_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug64_mutex1);
        pthread_mutex_unlock(&_bug64_mutex2);
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
    static const uint32_t indices[] = {0x63};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xbf34bd79, 0x4d405454};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11830ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug64_input_helper.interleave_num = 3;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0xad8d427b;
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
    static const uint32_t indices[] = {0x6f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x5c490b1d, 0xa1c7abaa};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x14a4ed19};
    static racebench_before_helper before_helpers[3] = {};
    _bug65_input_helper.interleave_num = 3;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0x12b5a3e0;
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
static volatile __attribute__((used)) int _bug66_ok;
static volatile __attribute__((used)) int* volatile _bug66_ptr;
static volatile __attribute__((used)) int _bug66_val;
static void _bug66_pattern_clean(void)
{
    _bug66_ok = 0;
    _bug66_ptr = NULL;
    _bug66_val = 0;
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug66_ok = 1;
        __sync_synchronize();
        _bug66_ptr = &_bug66_val;
    }
    else {
        while (!_bug66_ok)
            pthread_yield();
        if (_bug66_ptr == NULL)
            trigger = 1;
        _bug66_ptr = NULL;
        _bug66_ok = 0;
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
    static const uint32_t indices[] = {0xc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xcfc69b18};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa753f9ca};
    static racebench_before_helper before_helpers[2] = {};
    _bug66_input_helper.interleave_num = 2;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0x771a94e2;
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
    static const uint32_t indices[] = {0x6e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cae};
    static racebench_before_helper before_helpers[0] = {};
    _bug67_input_helper.interleave_num = 0;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0xa1182cae;
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
static volatile __attribute__((used)) int _bug68_arr[3];
static volatile __attribute__((used)) int _bug68_val;
static volatile __attribute__((used)) int _bug68_pred;
static void _bug68_pattern_clean(void)
{
    _bug68_arr[0] = 0, _bug68_arr[1] = 2, _bug68_arr[2] = 1;
    _bug68_val = 0;
    _bug68_pred = 0;
}
static __attribute__((optimize(0))) int _bug68_pattern(int part)
{
    int trigger = 0;
    _bug68_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug68_val += _bug68_arr[i];
        }
        __sync_synchronize();
        while (!_bug68_pred)
            pthread_yield();
        trigger = _bug68_val != 3;
        _bug68_pred = 0;
        _bug68_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug68_val += _bug68_arr[i];
        }
        __sync_synchronize();
        _bug68_pred = 1;
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
    static const uint32_t indices[] = {0x33};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x494eb2b7, 0x55685fac};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f0e};
    static racebench_before_helper before_helpers[3] = {};
    _bug68_input_helper.interleave_num = 3;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0x3fcf4171;
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
static volatile __attribute__((used)) int _bug69_ok;
static volatile __attribute__((used)) int* volatile _bug69_ptr;
static volatile __attribute__((used)) int _bug69_val;
static void _bug69_pattern_clean(void)
{
    _bug69_ok = 0;
    _bug69_ptr = NULL;
    _bug69_val = 0;
}
static __attribute__((optimize(0))) int _bug69_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug69_ok = 1;
        __sync_synchronize();
        _bug69_ptr = &_bug69_val;
    }
    else {
        while (!_bug69_ok)
            pthread_yield();
        if (_bug69_ptr == NULL)
            trigger = 1;
        _bug69_ptr = NULL;
        _bug69_ok = 0;
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
    static const uint32_t indices[] = {0x21};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xe128e2cc, 0xd30ad55f};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x94a510cb};
    static racebench_before_helper before_helpers[3] = {};
    _bug69_input_helper.interleave_num = 3;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0x48d8c8f6;
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
    static const uint32_t indices[] = {0x35};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x87038760, 0x69a8c566};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x5286ce75};
    static racebench_before_helper before_helpers[3] = {};
    _bug70_input_helper.interleave_num = 3;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0x43331b3b;
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
static pthread_cond_t _bug71_cond1;
static volatile __attribute__((used)) int _bug71_pred;
static volatile __attribute__((used)) int _bug71_sent;
static void _bug71_pattern_clean(void)
{
    pthread_mutex_init(&_bug71_mutex1, NULL);
    pthread_cond_init(&_bug71_cond1, NULL);
    _bug71_pred = 0;
    _bug71_sent = 0;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug71_mutex1);
        _bug71_pred = 1;
        pthread_cond_signal(&_bug71_cond1);
        _bug71_sent = 1;
        pthread_mutex_unlock(&_bug71_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug71_mutex1);
        if (_bug71_sent) {
            trigger = 1;
        }
        else {
            while (_bug71_pred == 0)
                pthread_cond_wait(&_bug71_cond1, &_bug71_mutex1);
        }
        _bug71_pred = 0;
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
    static const uint32_t indices[] = {0x85};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x416ff60b, 0xcdc04b0f};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118307e};
    static racebench_before_helper before_helpers[3] = {};
    _bug71_input_helper.interleave_num = 3;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0xb0487198;
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
static volatile __attribute__((used)) int _bug72_ok;
static volatile __attribute__((used)) int* volatile _bug72_ptr;
static volatile __attribute__((used)) int _bug72_val;
static void _bug72_pattern_clean(void)
{
    _bug72_ok = 0;
    _bug72_ptr = NULL;
    _bug72_val = 0;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug72_ok = 1;
        __sync_synchronize();
        _bug72_ptr = &_bug72_val;
    }
    else {
        while (!_bug72_ok)
            pthread_yield();
        if (_bug72_ptr == NULL)
            trigger = 1;
        _bug72_ptr = NULL;
        _bug72_ok = 0;
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
    static const uint32_t indices[] = {0x18};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182d0e};
    static racebench_before_helper before_helpers[0] = {};
    _bug72_input_helper.interleave_num = 0;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xa1182d0e;
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
static pthread_cond_t _bug73_cond1;
static volatile __attribute__((used)) int _bug73_pred;
static volatile __attribute__((used)) int _bug73_sent;
static void _bug73_pattern_clean(void)
{
    pthread_mutex_init(&_bug73_mutex1, NULL);
    pthread_cond_init(&_bug73_cond1, NULL);
    _bug73_pred = 0;
    _bug73_sent = 0;
}
static __attribute__((optimize(0))) int _bug73_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug73_mutex1);
        _bug73_pred = 1;
        pthread_cond_signal(&_bug73_cond1);
        _bug73_sent = 1;
        pthread_mutex_unlock(&_bug73_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug73_mutex1);
        if (_bug73_sent) {
            trigger = 1;
        }
        else {
            while (_bug73_pred == 0)
                pthread_cond_wait(&_bug73_cond1, &_bug73_mutex1);
        }
        _bug73_pred = 0;
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
    static const uint32_t indices[] = {0x5a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x71086179, 0x79201305};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118300e};
    static racebench_before_helper before_helpers[3] = {};
    _bug73_input_helper.interleave_num = 3;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0x8b40a48c;
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
static volatile __attribute__((used)) int _bug74_first_part;
static void _bug74_pattern_clean(void)
{
    pthread_mutex_init(&_bug74_mutex1, NULL);
    _bug74_first_part = -1;
}
static __attribute__((optimize(0))) int _bug74_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug74_mutex1);
        if (_bug74_first_part == -1)
            _bug74_first_part = part;
        else
            _bug74_first_part = -1;
        pthread_mutex_unlock(&_bug74_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug74_mutex1);
        if (_bug74_first_part == -1) {
            _bug74_first_part = part;
            trigger = 1;
        }
        else
            _bug74_first_part = -1;
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
    static const uint32_t indices[] = {0x64};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xa88b7dff, 0x9ce63405};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d2e};
    static racebench_before_helper before_helpers[3] = {};
    _bug74_input_helper.interleave_num = 3;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0xe689df32;
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
static volatile __attribute__((used)) int _bug75_val;
static void _bug75_pattern_clean(void)
{
    pthread_mutex_init(&_bug75_mutex1, NULL);
    _bug75_val = 0;
}
static __attribute__((optimize(0))) int _bug75_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug75_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug75_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug75_mutex1);
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

static void _bug75_pattern_clean(void);
static int _bug75_pattern(int part);
static racebench_bug_helper _bug75_bug_helper;
static racebench_input_helper _bug75_input_helper;
__attribute__((constructor))
static void _bug75_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1d};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e0e};
    static racebench_before_helper before_helpers[0] = {};
    _bug75_input_helper.interleave_num = 0;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xa1182e0e;
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
    static const uint32_t indices[] = {0x1b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x37d9a311};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x86b5bfdb};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0xbe8f62ec;
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
static pthread_mutex_t _bug77_mutex1;
static pthread_cond_t _bug77_cond1;
static volatile __attribute__((used)) int* volatile _bug77_ptr;
static volatile __attribute__((used)) int _bug77_val;
static volatile __attribute__((used)) int _bug77_pred;
static void _bug77_pattern_clean(void)
{
    pthread_mutex_init(&_bug77_mutex1, NULL);
    pthread_cond_init(&_bug77_cond1, NULL);
    _bug77_ptr = NULL;
    _bug77_val = 0;
    _bug77_pred = 0;
}
static __attribute__((optimize(0))) int _bug77_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug77_pred = 0;
        pthread_cond_signal(&_bug77_cond1);
        pthread_mutex_lock(&_bug77_mutex1);
        _bug77_ptr = &_bug77_val;
        _bug77_pred = 1;
        pthread_mutex_unlock(&_bug77_mutex1);
        pthread_cond_signal(&_bug77_cond1);
    }
    else {
        pthread_mutex_lock(&_bug77_mutex1);
        if (!_bug77_pred)
            pthread_cond_wait(&_bug77_cond1, &_bug77_mutex1);
        if (_bug77_ptr == NULL)
            trigger = 1;
        _bug77_ptr = NULL;
        _bug77_pred = 0;
        pthread_mutex_unlock(&_bug77_mutex1);
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
    static const uint32_t indices[] = {0x6};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x83428694, 0x956b769c};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115fc7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug77_input_helper.interleave_num = 3;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0xb9c3f9ae;
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
    static const uint32_t indices[] = {0x6a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[1] = {};
    _bug78_input_helper.interleave_num = 1;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0xa1182cce;
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
static volatile __attribute__((used)) int _bug79_val;
static void _bug79_pattern_clean(void)
{
    pthread_mutex_init(&_bug79_mutex1, NULL);
    _bug79_val = 0;
}
static __attribute__((optimize(0))) int _bug79_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug79_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug79_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug79_mutex1);
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

static void _bug79_pattern_clean(void);
static int _bug79_pattern(int part);
static racebench_bug_helper _bug79_bug_helper;
static racebench_input_helper _bug79_input_helper;
__attribute__((constructor))
static void _bug79_input_helper_init(void)
{
    static const uint32_t indices[] = {0x32};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa115df3e};
    static racebench_before_helper before_helpers[1] = {};
    _bug79_input_helper.interleave_num = 1;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0xa115df3e;
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
static volatile __attribute__((used)) int _bug80_arr[3];
static volatile __attribute__((used)) int _bug80_val;
static volatile __attribute__((used)) int _bug80_pred;
static void _bug80_pattern_clean(void)
{
    _bug80_arr[0] = 0, _bug80_arr[1] = 2, _bug80_arr[2] = 1;
    _bug80_val = 0;
    _bug80_pred = 0;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    _bug80_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug80_val += _bug80_arr[i];
        }
        __sync_synchronize();
        while (!_bug80_pred)
            pthread_yield();
        trigger = _bug80_val != 3;
        _bug80_pred = 0;
        _bug80_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug80_val += _bug80_arr[i];
        }
        __sync_synchronize();
        _bug80_pred = 1;
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
    static const uint32_t indices[] = {0x31};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x2772dd61};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118293e};
    static racebench_before_helper before_helpers[2] = {};
    _bug80_input_helper.interleave_num = 2;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xc88b069f;
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
static volatile __attribute__((used)) int _bug81_first_part;
static volatile __attribute__((used)) int _bug81_released;
static void _bug81_pattern_clean(void)
{
    pthread_mutex_init(&_bug81_mutex1, NULL);
    _bug81_first_part = -1;
    _bug81_released = -1;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug81_mutex1) == 0)
                break;
            if (_bug81_released == 0) {
                _bug81_first_part = -1;
                _bug81_released = -1;
                return 1;
            }
        }
        if (_bug81_first_part == -1)
            _bug81_first_part = part;
        else {
            _bug81_first_part = -1;
            while (_bug81_released == -1) {}
            _bug81_released = -1;
        }
        pthread_mutex_unlock(&_bug81_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug81_mutex1);
        if (_bug81_first_part == -1) {
            _bug81_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug81_mutex1);
                _bug81_released = 1;
            }
            else {
                _bug81_released = 0;
                while (_bug81_released != -1) {}
                pthread_mutex_unlock(&_bug81_mutex1);
                return 1;
            }
        }
        else {
            _bug81_first_part = -1;
            pthread_mutex_unlock(&_bug81_mutex1);
        }
    }
    return 0;
}

static void _bug81_pattern_clean(void);
static int _bug81_pattern(int part);
static racebench_bug_helper _bug81_bug_helper;
static racebench_input_helper _bug81_input_helper;
__attribute__((constructor))
static void _bug81_input_helper_init(void)
{
    static const uint32_t indices[] = {0x5a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x45f296f9};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[2] = {};
    _bug81_input_helper.interleave_num = 2;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xe70ac3c7;
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
static volatile __attribute__((used)) int _bug82_ok;
static volatile __attribute__((used)) int* volatile _bug82_ptr;
static volatile __attribute__((used)) int _bug82_val;
static void _bug82_pattern_clean(void)
{
    _bug82_ok = 0;
    _bug82_ptr = NULL;
    _bug82_val = 0;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug82_ok = 1;
        __sync_synchronize();
        _bug82_ptr = &_bug82_val;
    }
    else {
        while (!_bug82_ok)
            pthread_yield();
        if (_bug82_ptr == NULL)
            trigger = 1;
        _bug82_ptr = NULL;
        _bug82_ok = 0;
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
    static const uint32_t indices[] = {0x10};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4e2c82b3, 0x63d6c725};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11b2cde};
    static racebench_before_helper before_helpers[3] = {};
    _bug82_input_helper.interleave_num = 3;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0x531e76b6;
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
static volatile __attribute__((used)) int _bug83_first_part;
static volatile __attribute__((used)) int _bug83_released;
static void _bug83_pattern_clean(void)
{
    pthread_mutex_init(&_bug83_mutex1, NULL);
    _bug83_first_part = -1;
    _bug83_released = -1;
}
static __attribute__((optimize(0))) int _bug83_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug83_mutex1) == 0)
                break;
            if (_bug83_released == 0) {
                _bug83_first_part = -1;
                _bug83_released = -1;
                return 1;
            }
        }
        if (_bug83_first_part == -1)
            _bug83_first_part = part;
        else {
            _bug83_first_part = -1;
            while (_bug83_released == -1) {}
            _bug83_released = -1;
        }
        pthread_mutex_unlock(&_bug83_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug83_mutex1);
        if (_bug83_first_part == -1) {
            _bug83_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug83_mutex1);
                _bug83_released = 1;
            }
            else {
                _bug83_released = 0;
                while (_bug83_released != -1) {}
                pthread_mutex_unlock(&_bug83_mutex1);
                return 1;
            }
        }
        else {
            _bug83_first_part = -1;
            pthread_mutex_unlock(&_bug83_mutex1);
        }
    }
    return 0;
}

static void _bug83_pattern_clean(void);
static int _bug83_pattern(int part);
static racebench_bug_helper _bug83_bug_helper;
static racebench_input_helper _bug83_input_helper;
__attribute__((constructor))
static void _bug83_input_helper_init(void)
{
    static const uint32_t indices[] = {0x18};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x2ec8afa1};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug83_input_helper.interleave_num = 2;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0xcfe0dd1f;
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
static pthread_mutex_t _bug84_mutex2;
static void _bug84_pattern_clean(void)
{
    pthread_mutex_init(&_bug84_mutex1, NULL);
    pthread_mutex_init(&_bug84_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug84_mutex1);
        if (pthread_mutex_trylock(&_bug84_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug84_mutex2);
        pthread_mutex_unlock(&_bug84_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug84_mutex2);
        if (pthread_mutex_trylock(&_bug84_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug84_mutex1);
        pthread_mutex_unlock(&_bug84_mutex2);
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
    static const uint32_t indices[] = {0x38};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118234e};
    static racebench_before_helper before_helpers[0] = {};
    _bug84_input_helper.interleave_num = 0;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xa118234e;
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
    static const uint32_t indices[] = {0x6b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x23d58019};
    static racebench_before_helper before_helpers[1] = {};
    _bug85_input_helper.interleave_num = 1;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0x23d58019;
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
static volatile __attribute__((used)) int _bug86_ok;
static volatile __attribute__((used)) int* volatile _bug86_ptr;
static volatile __attribute__((used)) int _bug86_val;
static void _bug86_pattern_clean(void)
{
    _bug86_ok = 0;
    _bug86_ptr = NULL;
    _bug86_val = 0;
}
static __attribute__((optimize(0))) int _bug86_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug86_ok = 1;
        __sync_synchronize();
        _bug86_ptr = &_bug86_val;
    }
    else {
        while (!_bug86_ok)
            pthread_yield();
        if (_bug86_ptr == NULL)
            trigger = 1;
        _bug86_ptr = NULL;
        _bug86_ok = 0;
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
    static const uint32_t indices[] = {0x1d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7bc31657};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1168c7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug86_input_helper.interleave_num = 2;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x1cd9a2d5;
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
static pthread_cond_t _bug87_cond1;
static volatile __attribute__((used)) int* volatile _bug87_ptr;
static volatile __attribute__((used)) int _bug87_val;
static volatile __attribute__((used)) int _bug87_pred;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    pthread_cond_init(&_bug87_cond1, NULL);
    _bug87_ptr = NULL;
    _bug87_val = 0;
    _bug87_pred = 0;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug87_pred = 0;
        pthread_cond_signal(&_bug87_cond1);
        pthread_mutex_lock(&_bug87_mutex1);
        _bug87_ptr = &_bug87_val;
        _bug87_pred = 1;
        pthread_mutex_unlock(&_bug87_mutex1);
        pthread_cond_signal(&_bug87_cond1);
    }
    else {
        pthread_mutex_lock(&_bug87_mutex1);
        if (!_bug87_pred)
            pthread_cond_wait(&_bug87_cond1, &_bug87_mutex1);
        if (_bug87_ptr == NULL)
            trigger = 1;
        _bug87_ptr = NULL;
        _bug87_pred = 0;
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
    static const uint32_t indices[] = {0x3};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa4845d4f};
    static racebench_before_helper before_helpers[0] = {};
    _bug87_input_helper.interleave_num = 0;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0xa4845d4f;
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
static volatile __attribute__((used)) int* volatile _bug88_ptr;
static volatile __attribute__((used)) int _bug88_val;
static void _bug88_pattern_clean(void)
{
    pthread_mutex_init(&_bug88_mutex1, NULL);
    _bug88_ptr = NULL;
    _bug88_val = 0;
}
static __attribute__((optimize(0))) int _bug88_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug88_mutex1);
        _bug88_ptr = &_bug88_val;
        pthread_mutex_unlock(&_bug88_mutex1);
        pthread_mutex_lock(&_bug88_mutex1);
        if (_bug88_ptr == NULL)
            trigger = 1;
        _bug88_ptr = NULL;
        pthread_mutex_unlock(&_bug88_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug88_mutex1);
        _bug88_ptr = &_bug88_val;
        if (_bug88_ptr == NULL)
            trigger = 1;
        _bug88_ptr = NULL;
        pthread_mutex_unlock(&_bug88_mutex1);
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
    static const uint32_t indices[] = {0x43};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa114bfee};
    static racebench_before_helper before_helpers[0] = {};
    _bug88_input_helper.interleave_num = 0;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0xa114bfee;
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
    static const uint32_t indices[] = {0x80};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x631713d6, 0xb4e1c970};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11f5cbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug89_input_helper.interleave_num = 3;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0xb9183a04;
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
    static const uint32_t indices[] = {0x40};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x640b4a9, 0xb43b2d37};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182fee};
    static racebench_before_helper before_helpers[3] = {};
    _bug90_input_helper.interleave_num = 3;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0x5b9411ce;
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
static volatile __attribute__((used)) int* volatile _bug91_ptr;
static volatile __attribute__((used)) int _bug91_val;
static void _bug91_pattern_clean(void)
{
    pthread_mutex_init(&_bug91_mutex1, NULL);
    _bug91_ptr = NULL;
    _bug91_val = 0;
}
static __attribute__((optimize(0))) int _bug91_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug91_mutex1);
        _bug91_ptr = &_bug91_val;
        pthread_mutex_unlock(&_bug91_mutex1);
        pthread_mutex_lock(&_bug91_mutex1);
        if (_bug91_ptr == NULL)
            trigger = 1;
        _bug91_ptr = NULL;
        pthread_mutex_unlock(&_bug91_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug91_mutex1);
        _bug91_ptr = &_bug91_val;
        if (_bug91_ptr == NULL)
            trigger = 1;
        _bug91_ptr = NULL;
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
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb2d87d46};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x3446bd8f};
    static racebench_before_helper before_helpers[2] = {};
    _bug91_input_helper.interleave_num = 2;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0xe71f3ad5;
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
static pthread_cond_t _bug92_cond1;
static volatile __attribute__((used)) int* volatile _bug92_ptr;
static volatile __attribute__((used)) int _bug92_val;
static volatile __attribute__((used)) int _bug92_pred;
static void _bug92_pattern_clean(void)
{
    pthread_mutex_init(&_bug92_mutex1, NULL);
    pthread_cond_init(&_bug92_cond1, NULL);
    _bug92_ptr = NULL;
    _bug92_val = 0;
    _bug92_pred = 0;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug92_pred = 0;
        pthread_cond_signal(&_bug92_cond1);
        pthread_mutex_lock(&_bug92_mutex1);
        _bug92_ptr = &_bug92_val;
        _bug92_pred = 1;
        pthread_mutex_unlock(&_bug92_mutex1);
        pthread_cond_signal(&_bug92_cond1);
    }
    else {
        pthread_mutex_lock(&_bug92_mutex1);
        if (!_bug92_pred)
            pthread_cond_wait(&_bug92_cond1, &_bug92_mutex1);
        if (_bug92_ptr == NULL)
            trigger = 1;
        _bug92_ptr = NULL;
        _bug92_pred = 0;
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
    static const uint32_t indices[] = {0x6b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x76e537eb};
    static racebench_before_helper before_helpers[0] = {};
    _bug92_input_helper.interleave_num = 0;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0x76e537eb;
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
    static const uint32_t indices[] = {0x36};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1155ffe};
    static racebench_before_helper before_helpers[0] = {};
    _bug93_input_helper.interleave_num = 0;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0xa1155ffe;
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
static volatile __attribute__((used)) int _bug94_pred;
static volatile __attribute__((used)) int _bug94_sent;
static void _bug94_pattern_clean(void)
{
    pthread_mutex_init(&_bug94_mutex1, NULL);
    pthread_cond_init(&_bug94_cond1, NULL);
    _bug94_pred = 0;
    _bug94_sent = 0;
}
static __attribute__((optimize(0))) int _bug94_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug94_mutex1);
        _bug94_pred = 1;
        pthread_cond_signal(&_bug94_cond1);
        _bug94_sent = 1;
        pthread_mutex_unlock(&_bug94_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug94_mutex1);
        if (_bug94_sent) {
            trigger = 1;
        }
        else {
            while (_bug94_pred == 0)
                pthread_cond_wait(&_bug94_cond1, &_bug94_mutex1);
        }
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
    static const uint32_t indices[] = {0x17};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdcb6eb8b, 0x50a669da};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1155cae};
    static racebench_before_helper before_helpers[3] = {};
    _bug94_input_helper.interleave_num = 3;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0xce72b213;
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
    static const uint32_t indices[] = {0x3f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x6bc59b3b};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1142ace};
    static racebench_before_helper before_helpers[2] = {};
    _bug95_input_helper.interleave_num = 2;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xcd9c609;
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
static volatile __attribute__((used)) int _bug96_arr[3];
static volatile __attribute__((used)) int _bug96_val;
static volatile __attribute__((used)) int _bug96_pred;
static void _bug96_pattern_clean(void)
{
    _bug96_arr[0] = 0, _bug96_arr[1] = 2, _bug96_arr[2] = 1;
    _bug96_val = 0;
    _bug96_pred = 0;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    _bug96_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug96_val += _bug96_arr[i];
        }
        __sync_synchronize();
        while (!_bug96_pred)
            pthread_yield();
        trigger = _bug96_val != 3;
        _bug96_pred = 0;
        _bug96_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug96_val += _bug96_arr[i];
        }
        __sync_synchronize();
        _bug96_pred = 1;
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
    static const uint32_t indices[] = {0x19};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x3365e7d9, 0x55698894};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x32c5b3d9};
    static racebench_before_helper before_helpers[3] = {};
    _bug96_input_helper.interleave_num = 3;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xbb952446;
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
    static const uint32_t indices[] = {0x12};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xaf87b1ad};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115ef9e};
    static racebench_before_helper before_helpers[2] = {};
    _bug97_input_helper.interleave_num = 2;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0x509da14b;
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
static volatile __attribute__((used)) int _bug98_first_part;
static volatile __attribute__((used)) int _bug98_released;
static void _bug98_pattern_clean(void)
{
    pthread_mutex_init(&_bug98_mutex1, NULL);
    _bug98_first_part = -1;
    _bug98_released = -1;
}
static __attribute__((optimize(0))) int _bug98_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug98_mutex1) == 0)
                break;
            if (_bug98_released == 0) {
                _bug98_first_part = -1;
                _bug98_released = -1;
                return 1;
            }
        }
        if (_bug98_first_part == -1)
            _bug98_first_part = part;
        else {
            _bug98_first_part = -1;
            while (_bug98_released == -1) {}
            _bug98_released = -1;
        }
        pthread_mutex_unlock(&_bug98_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug98_mutex1);
        if (_bug98_first_part == -1) {
            _bug98_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug98_mutex1);
                _bug98_released = 1;
            }
            else {
                _bug98_released = 0;
                while (_bug98_released != -1) {}
                pthread_mutex_unlock(&_bug98_mutex1);
                return 1;
            }
        }
        else {
            _bug98_first_part = -1;
            pthread_mutex_unlock(&_bug98_mutex1);
        }
    }
    return 0;
}

static void _bug98_pattern_clean(void);
static int _bug98_pattern(int part);
static racebench_bug_helper _bug98_bug_helper;
static racebench_input_helper _bug98_input_helper;
__attribute__((constructor))
static void _bug98_input_helper_init(void)
{
    static const uint32_t indices[] = {0x57};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xdf6d25df};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug98_input_helper.interleave_num = 2;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0x8085521d;
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
static volatile __attribute__((used)) int _bug99_arr[3];
static volatile __attribute__((used)) int _bug99_val;
static volatile __attribute__((used)) int _bug99_pred;
static void _bug99_pattern_clean(void)
{
    _bug99_arr[0] = 0, _bug99_arr[1] = 2, _bug99_arr[2] = 1;
    _bug99_val = 0;
    _bug99_pred = 0;
}
static __attribute__((optimize(0))) int _bug99_pattern(int part)
{
    int trigger = 0;
    _bug99_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug99_val += _bug99_arr[i];
        }
        __sync_synchronize();
        while (!_bug99_pred)
            pthread_yield();
        trigger = _bug99_val != 3;
        _bug99_pred = 0;
        _bug99_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug99_val += _bug99_arr[i];
        }
        __sync_synchronize();
        _bug99_pred = 1;
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
    static const uint32_t indices[] = {0x63};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf51781d9};
    static racebench_before_helper before_helpers[0] = {};
    _bug99_input_helper.interleave_num = 0;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0xf51781d9;
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