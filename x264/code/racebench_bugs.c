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
    static const uint32_t indices[] = {0x1ea5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x4dd85a86, 0x13f42210};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x5602aa71};
    static racebench_before_helper before_helpers[3] = {};
    _bug0_input_helper.interleave_num = 3;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0xb7cf2707;
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
static volatile __attribute__((used)) int _bug1_first_part;
static volatile __attribute__((used)) int _bug1_released;
static void _bug1_pattern_clean(void)
{
    pthread_mutex_init(&_bug1_mutex1, NULL);
    _bug1_first_part = -1;
    _bug1_released = -1;
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug1_mutex1) == 0)
                break;
            if (_bug1_released == 0) {
                _bug1_first_part = -1;
                _bug1_released = -1;
                return 1;
            }
        }
        if (_bug1_first_part == -1)
            _bug1_first_part = part;
        else {
            _bug1_first_part = -1;
            while (_bug1_released == -1) {}
            _bug1_released = -1;
        }
        pthread_mutex_unlock(&_bug1_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug1_mutex1);
        if (_bug1_first_part == -1) {
            _bug1_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug1_mutex1);
                _bug1_released = 1;
            }
            else {
                _bug1_released = 0;
                while (_bug1_released != -1) {}
                pthread_mutex_unlock(&_bug1_mutex1);
                return 1;
            }
        }
        else {
            _bug1_first_part = -1;
            pthread_mutex_unlock(&_bug1_mutex1);
        }
    }
    return 0;
}

static void _bug1_pattern_clean(void);
static int _bug1_pattern(int part);
static racebench_bug_helper _bug1_bug_helper;
static racebench_input_helper _bug1_input_helper;
__attribute__((constructor))
static void _bug1_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2d0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xc2e2bca0};
    static racebench_before_helper before_helpers[1] = {};
    _bug1_input_helper.interleave_num = 1;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xc2e2bca0;
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
static volatile __attribute__((used)) int _bug2_first_part;
static void _bug2_pattern_clean(void)
{
    pthread_mutex_init(&_bug2_mutex1, NULL);
    _bug2_first_part = -1;
}
static __attribute__((optimize(0))) int _bug2_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug2_mutex1);
        if (_bug2_first_part == -1)
            _bug2_first_part = part;
        else
            _bug2_first_part = -1;
        pthread_mutex_unlock(&_bug2_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug2_mutex1);
        if (_bug2_first_part == -1) {
            _bug2_first_part = part;
            trigger = 1;
        }
        else
            _bug2_first_part = -1;
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
    static const uint32_t indices[] = {0x244e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x628685ca};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug2_input_helper.interleave_num = 2;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0x39eb238;
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
    static const uint32_t indices[] = {0x1cb1};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1169ede};
    static racebench_before_helper before_helpers[0] = {};
    _bug3_input_helper.interleave_num = 0;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xa1169ede;
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
    static const uint32_t indices[] = {0x104};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1129c6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug4_input_helper.interleave_num = 1;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xa1129c6e;
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
static volatile __attribute__((used)) int _bug5_arr[3];
static volatile __attribute__((used)) int _bug5_val;
static volatile __attribute__((used)) int _bug5_pred;
static void _bug5_pattern_clean(void)
{
    _bug5_arr[0] = 0, _bug5_arr[1] = 2, _bug5_arr[2] = 1;
    _bug5_val = 0;
    _bug5_pred = 0;
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    int trigger = 0;
    _bug5_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug5_val += _bug5_arr[i];
        }
        __sync_synchronize();
        while (!_bug5_pred)
            pthread_yield();
        trigger = _bug5_val != 3;
        _bug5_pred = 0;
        _bug5_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug5_val += _bug5_arr[i];
        }
        __sync_synchronize();
        _bug5_pred = 1;
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
    static const uint32_t indices[] = {0x109d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf325413f};
    static racebench_before_helper before_helpers[0] = {};
    _bug5_input_helper.interleave_num = 0;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xf325413f;
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
    static const uint32_t indices[] = {0x5ad};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x96e046a6, 0x65b7e7dd};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182dee};
    static racebench_before_helper before_helpers[3] = {};
    _bug6_input_helper.interleave_num = 3;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0x9db05c71;
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
    static const uint32_t indices[] = {0x2686};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1168c7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug7_input_helper.interleave_num = 0;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xa1168c7e;
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
static volatile __attribute__((used)) int _bug8_ok;
static volatile __attribute__((used)) int* volatile _bug8_ptr;
static volatile __attribute__((used)) int _bug8_val;
static void _bug8_pattern_clean(void)
{
    _bug8_ok = 0;
    _bug8_ptr = NULL;
    _bug8_val = 0;
}
static __attribute__((optimize(0))) int _bug8_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug8_ok = 1;
        __sync_synchronize();
        _bug8_ptr = &_bug8_val;
    }
    else {
        while (!_bug8_ok)
            pthread_yield();
        if (_bug8_ptr == NULL)
            trigger = 1;
        _bug8_ptr = NULL;
        _bug8_ok = 0;
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
    static const uint32_t indices[] = {0x1104};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x81c56476};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11825fe};
    static racebench_before_helper before_helpers[2] = {};
    _bug8_input_helper.interleave_num = 2;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0x22dd8a74;
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
    static const uint32_t indices[] = {0xa6b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug9_input_helper.interleave_num = 1;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0xa1182c4e;
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
    static const uint32_t indices[] = {0x12a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xf86e73ba, 0xe4183ea4};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182e3e};
    static racebench_before_helper before_helpers[3] = {};
    _bug10_input_helper.interleave_num = 3;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0x7d9ee09c;
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
    static const uint32_t indices[] = {0xda8};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xf12667c9, 0x926dca17};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118254e};
    static racebench_before_helper before_helpers[3] = {};
    _bug11_input_helper.interleave_num = 3;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0x24ac572e;
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
static volatile __attribute__((used)) int _bug12_val;
static void _bug12_pattern_clean(void)
{
    pthread_mutex_init(&_bug12_mutex1, NULL);
    _bug12_val = 0;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug12_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug12_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug12_mutex1);
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

static void _bug12_pattern_clean(void);
static int _bug12_pattern(int part);
static racebench_bug_helper _bug12_bug_helper;
static racebench_input_helper _bug12_input_helper;
__attribute__((constructor))
static void _bug12_input_helper_init(void)
{
    static const uint32_t indices[] = {0x27b5};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c3e};
    static racebench_before_helper before_helpers[0] = {};
    _bug12_input_helper.interleave_num = 0;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xa1182c3e;
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
static volatile __attribute__((used)) int _bug13_ok;
static volatile __attribute__((used)) int* volatile _bug13_ptr;
static volatile __attribute__((used)) int _bug13_val;
static void _bug13_pattern_clean(void)
{
    _bug13_ok = 0;
    _bug13_ptr = NULL;
    _bug13_val = 0;
}
static __attribute__((optimize(0))) int _bug13_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug13_ok = 1;
        __sync_synchronize();
        _bug13_ptr = &_bug13_val;
    }
    else {
        while (!_bug13_ok)
            pthread_yield();
        if (_bug13_ptr == NULL)
            trigger = 1;
        _bug13_ptr = NULL;
        _bug13_ok = 0;
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
    static const uint32_t indices[] = {0x1fc1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182a6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug13_input_helper.interleave_num = 0;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0xa1182a6e;
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
static volatile __attribute__((used)) int _bug14_ok;
static volatile __attribute__((used)) int* volatile _bug14_ptr;
static volatile __attribute__((used)) int _bug14_val;
static void _bug14_pattern_clean(void)
{
    _bug14_ok = 0;
    _bug14_ptr = NULL;
    _bug14_val = 0;
}
static __attribute__((optimize(0))) int _bug14_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug14_ok = 1;
        __sync_synchronize();
        _bug14_ptr = &_bug14_val;
    }
    else {
        while (!_bug14_ok)
            pthread_yield();
        if (_bug14_ptr == NULL)
            trigger = 1;
        _bug14_ptr = NULL;
        _bug14_ok = 0;
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
    static const uint32_t indices[] = {0x968};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xbde23941, 0x6fef187};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x45202b9f};
    static racebench_before_helper before_helpers[3] = {};
    _bug14_input_helper.interleave_num = 3;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xa015667;
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
static volatile __attribute__((used)) int _bug15_ok;
static volatile __attribute__((used)) int* volatile _bug15_ptr;
static volatile __attribute__((used)) int _bug15_val;
static void _bug15_pattern_clean(void)
{
    _bug15_ok = 0;
    _bug15_ptr = NULL;
    _bug15_val = 0;
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug15_ok = 1;
        __sync_synchronize();
        _bug15_ptr = &_bug15_val;
    }
    else {
        while (!_bug15_ok)
            pthread_yield();
        if (_bug15_ptr == NULL)
            trigger = 1;
        _bug15_ptr = NULL;
        _bug15_ok = 0;
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
    static const uint32_t indices[] = {0x82e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[1] = {};
    _bug15_input_helper.interleave_num = 1;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0xa1182c7e;
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
static volatile __attribute__((used)) int* volatile _bug16_ptr;
static volatile __attribute__((used)) int _bug16_val;
static void _bug16_pattern_clean(void)
{
    pthread_mutex_init(&_bug16_mutex1, NULL);
    _bug16_ptr = NULL;
    _bug16_val = 0;
}
static __attribute__((optimize(0))) int _bug16_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug16_mutex1);
        _bug16_ptr = &_bug16_val;
        pthread_mutex_unlock(&_bug16_mutex1);
        pthread_mutex_lock(&_bug16_mutex1);
        if (_bug16_ptr == NULL)
            trigger = 1;
        _bug16_ptr = NULL;
        pthread_mutex_unlock(&_bug16_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug16_mutex1);
        _bug16_ptr = &_bug16_val;
        if (_bug16_ptr == NULL)
            trigger = 1;
        _bug16_ptr = NULL;
        pthread_mutex_unlock(&_bug16_mutex1);
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
    static const uint32_t indices[] = {0x515};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x66a8cf92};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11a9b7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug16_input_helper.interleave_num = 2;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0x7c36b10;
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
static volatile __attribute__((used)) int _bug17_first_part;
static void _bug17_pattern_clean(void)
{
    pthread_mutex_init(&_bug17_mutex1, NULL);
    _bug17_first_part = -1;
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug17_mutex1);
        if (_bug17_first_part == -1)
            _bug17_first_part = part;
        else
            _bug17_first_part = -1;
        pthread_mutex_unlock(&_bug17_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug17_mutex1);
        if (_bug17_first_part == -1) {
            _bug17_first_part = part;
            trigger = 1;
        }
        else
            _bug17_first_part = -1;
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
    static const uint32_t indices[] = {0x1d2c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x32268ccf};
    static racebench_before_helper before_helpers[0] = {};
    _bug17_input_helper.interleave_num = 0;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0x32268ccf;
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
    static const uint32_t indices[] = {0x138d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x95417bd5};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x37653bbc};
    static racebench_before_helper before_helpers[2] = {};
    _bug18_input_helper.interleave_num = 2;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0xcca6b791;
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
static volatile __attribute__((used)) int _bug19_released;
static void _bug19_pattern_clean(void)
{
    pthread_mutex_init(&_bug19_mutex1, NULL);
    _bug19_first_part = -1;
    _bug19_released = -1;
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug19_mutex1) == 0)
                break;
            if (_bug19_released == 0) {
                _bug19_first_part = -1;
                _bug19_released = -1;
                return 1;
            }
        }
        if (_bug19_first_part == -1)
            _bug19_first_part = part;
        else {
            _bug19_first_part = -1;
            while (_bug19_released == -1) {}
            _bug19_released = -1;
        }
        pthread_mutex_unlock(&_bug19_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug19_mutex1);
        if (_bug19_first_part == -1) {
            _bug19_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug19_mutex1);
                _bug19_released = 1;
            }
            else {
                _bug19_released = 0;
                while (_bug19_released != -1) {}
                pthread_mutex_unlock(&_bug19_mutex1);
                return 1;
            }
        }
        else {
            _bug19_first_part = -1;
            pthread_mutex_unlock(&_bug19_mutex1);
        }
    }
    return 0;
}

static void _bug19_pattern_clean(void);
static int _bug19_pattern(int part);
static racebench_bug_helper _bug19_bug_helper;
static racebench_input_helper _bug19_input_helper;
__attribute__((constructor))
static void _bug19_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1daa};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x74818766};
    static racebench_before_helper before_helpers[0] = {};
    _bug19_input_helper.interleave_num = 0;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0x74818766;
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
    static const uint32_t indices[] = {0x25a2};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xb8443cb6};
    static racebench_before_helper before_helpers[1] = {};
    _bug20_input_helper.interleave_num = 1;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xb8443cb6;
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
static volatile __attribute__((used)) int _bug21_pred;
static volatile __attribute__((used)) int _bug21_sent;
static void _bug21_pattern_clean(void)
{
    pthread_mutex_init(&_bug21_mutex1, NULL);
    pthread_cond_init(&_bug21_cond1, NULL);
    _bug21_pred = 0;
    _bug21_sent = 0;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug21_mutex1);
        _bug21_pred = 1;
        pthread_cond_signal(&_bug21_cond1);
        _bug21_sent = 1;
        pthread_mutex_unlock(&_bug21_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug21_mutex1);
        if (_bug21_sent) {
            trigger = 1;
        }
        else {
            while (_bug21_pred == 0)
                pthread_cond_wait(&_bug21_cond1, &_bug21_mutex1);
        }
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
    static const uint32_t indices[] = {0x3bd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x95c9ebb, 0x5e3b8b6};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11448de};
    static racebench_before_helper before_helpers[3] = {};
    _bug21_input_helper.interleave_num = 3;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0xb054a04f;
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
static pthread_cond_t _bug22_cond1;
static volatile __attribute__((used)) int _bug22_pred;
static volatile __attribute__((used)) int _bug22_sent;
static void _bug22_pattern_clean(void)
{
    pthread_mutex_init(&_bug22_mutex1, NULL);
    pthread_cond_init(&_bug22_cond1, NULL);
    _bug22_pred = 0;
    _bug22_sent = 0;
}
static __attribute__((optimize(0))) int _bug22_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug22_mutex1);
        _bug22_pred = 1;
        pthread_cond_signal(&_bug22_cond1);
        _bug22_sent = 1;
        pthread_mutex_unlock(&_bug22_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug22_mutex1);
        if (_bug22_sent) {
            trigger = 1;
        }
        else {
            while (_bug22_pred == 0)
                pthread_cond_wait(&_bug22_cond1, &_bug22_mutex1);
        }
        _bug22_pred = 0;
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
    static const uint32_t indices[] = {0x21ca};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182a5e};
    static racebench_before_helper before_helpers[1] = {};
    _bug22_input_helper.interleave_num = 1;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0xa1182a5e;
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
    static const uint32_t indices[] = {0x100e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c6e};
    static racebench_before_helper before_helpers[1] = {};
    _bug23_input_helper.interleave_num = 1;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0xa1182c6e;
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
static volatile __attribute__((used)) int _bug24_arr[3];
static volatile __attribute__((used)) int _bug24_val;
static volatile __attribute__((used)) int _bug24_pred;
static void _bug24_pattern_clean(void)
{
    _bug24_arr[0] = 0, _bug24_arr[1] = 2, _bug24_arr[2] = 1;
    _bug24_val = 0;
    _bug24_pred = 0;
}
static __attribute__((optimize(0))) int _bug24_pattern(int part)
{
    int trigger = 0;
    _bug24_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug24_val += _bug24_arr[i];
        }
        __sync_synchronize();
        while (!_bug24_pred)
            pthread_yield();
        trigger = _bug24_val != 3;
        _bug24_pred = 0;
        _bug24_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug24_val += _bug24_arr[i];
        }
        __sync_synchronize();
        _bug24_pred = 1;
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
    static const uint32_t indices[] = {0xdc0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11827de};
    static racebench_before_helper before_helpers[0] = {};
    _bug24_input_helper.interleave_num = 0;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0xa11827de;
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
static volatile __attribute__((used)) int _bug25_first_part;
static void _bug25_pattern_clean(void)
{
    pthread_mutex_init(&_bug25_mutex1, NULL);
    _bug25_first_part = -1;
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug25_mutex1);
        if (_bug25_first_part == -1)
            _bug25_first_part = part;
        else
            _bug25_first_part = -1;
        pthread_mutex_unlock(&_bug25_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug25_mutex1);
        if (_bug25_first_part == -1) {
            _bug25_first_part = part;
            trigger = 1;
        }
        else
            _bug25_first_part = -1;
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
    static const uint32_t indices[] = {0x1a10};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa114b14e};
    static racebench_before_helper before_helpers[1] = {};
    _bug25_input_helper.interleave_num = 1;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xa114b14e;
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
static volatile __attribute__((used)) int _bug26_val;
static void _bug26_pattern_clean(void)
{
    pthread_mutex_init(&_bug26_mutex1, NULL);
    _bug26_val = 0;
}
static __attribute__((optimize(0))) int _bug26_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug26_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug26_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug26_mutex1);
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

static void _bug26_pattern_clean(void);
static int _bug26_pattern(int part);
static racebench_bug_helper _bug26_bug_helper;
static racebench_input_helper _bug26_input_helper;
__attribute__((constructor))
static void _bug26_input_helper_init(void)
{
    static const uint32_t indices[] = {0x9c0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf7f9d745, 0xc6a1784c};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x96a4f12f};
    static racebench_before_helper before_helpers[3] = {};
    _bug26_input_helper.interleave_num = 3;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0x554040c0;
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
static pthread_cond_t _bug27_cond1;
static volatile __attribute__((used)) int* volatile _bug27_ptr;
static volatile __attribute__((used)) int _bug27_val;
static volatile __attribute__((used)) int _bug27_pred;
static void _bug27_pattern_clean(void)
{
    pthread_mutex_init(&_bug27_mutex1, NULL);
    pthread_cond_init(&_bug27_cond1, NULL);
    _bug27_ptr = NULL;
    _bug27_val = 0;
    _bug27_pred = 0;
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug27_pred = 0;
        pthread_cond_signal(&_bug27_cond1);
        pthread_mutex_lock(&_bug27_mutex1);
        _bug27_ptr = &_bug27_val;
        _bug27_pred = 1;
        pthread_mutex_unlock(&_bug27_mutex1);
        pthread_cond_signal(&_bug27_cond1);
    }
    else {
        pthread_mutex_lock(&_bug27_mutex1);
        if (!_bug27_pred)
            pthread_cond_wait(&_bug27_cond1, &_bug27_mutex1);
        if (_bug27_ptr == NULL)
            trigger = 1;
        _bug27_ptr = NULL;
        _bug27_pred = 0;
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
    static const uint32_t indices[] = {0x452};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa5aeaefc, 0xd1624bee};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x14e6ef5f};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0x8bf7ea49;
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
static volatile __attribute__((used)) int _bug28_ok;
static volatile __attribute__((used)) int* volatile _bug28_ptr;
static volatile __attribute__((used)) int _bug28_val;
static void _bug28_pattern_clean(void)
{
    _bug28_ok = 0;
    _bug28_ptr = NULL;
    _bug28_val = 0;
}
static __attribute__((optimize(0))) int _bug28_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug28_ok = 1;
        __sync_synchronize();
        _bug28_ptr = &_bug28_val;
    }
    else {
        while (!_bug28_ok)
            pthread_yield();
        if (_bug28_ptr == NULL)
            trigger = 1;
        _bug28_ptr = NULL;
        _bug28_ok = 0;
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
    static const uint32_t indices[] = {0x1b31};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa113302e};
    static racebench_before_helper before_helpers[1] = {};
    _bug28_input_helper.interleave_num = 1;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa113302e;
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
    static const uint32_t indices[] = {0x9c0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x7305c02f};
    static racebench_before_helper before_helpers[1] = {};
    _bug29_input_helper.interleave_num = 1;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x7305c02f;
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
static pthread_cond_t _bug30_cond1;
static volatile __attribute__((used)) int _bug30_pred;
static volatile __attribute__((used)) int _bug30_sent;
static void _bug30_pattern_clean(void)
{
    pthread_mutex_init(&_bug30_mutex1, NULL);
    pthread_cond_init(&_bug30_cond1, NULL);
    _bug30_pred = 0;
    _bug30_sent = 0;
}
static __attribute__((optimize(0))) int _bug30_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug30_mutex1);
        _bug30_pred = 1;
        pthread_cond_signal(&_bug30_cond1);
        _bug30_sent = 1;
        pthread_mutex_unlock(&_bug30_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug30_mutex1);
        if (_bug30_sent) {
            trigger = 1;
        }
        else {
            while (_bug30_pred == 0)
                pthread_cond_wait(&_bug30_cond1, &_bug30_mutex1);
        }
        _bug30_pred = 0;
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
    static const uint32_t indices[] = {0x1300};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x235a852c};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x3b5ae40};
    static racebench_before_helper before_helpers[2] = {};
    _bug30_input_helper.interleave_num = 2;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0x2710336c;
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
static volatile __attribute__((used)) int _bug31_val;
static void _bug31_pattern_clean(void)
{
    pthread_mutex_init(&_bug31_mutex1, NULL);
    _bug31_val = 0;
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug31_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug31_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug31_mutex1);
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

static void _bug31_pattern_clean(void);
static int _bug31_pattern(int part);
static racebench_bug_helper _bug31_bug_helper;
static racebench_input_helper _bug31_input_helper;
__attribute__((constructor))
static void _bug31_input_helper_init(void)
{
    static const uint32_t indices[] = {0x27c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x98913caf};
    static racebench_before_helper before_helpers[0] = {};
    _bug31_input_helper.interleave_num = 0;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0x98913caf;
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
static volatile __attribute__((used)) int* volatile _bug32_ptr;
static volatile __attribute__((used)) int _bug32_val;
static void _bug32_pattern_clean(void)
{
    pthread_mutex_init(&_bug32_mutex1, NULL);
    _bug32_ptr = NULL;
    _bug32_val = 0;
}
static __attribute__((optimize(0))) int _bug32_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug32_mutex1);
        _bug32_ptr = &_bug32_val;
        pthread_mutex_unlock(&_bug32_mutex1);
        pthread_mutex_lock(&_bug32_mutex1);
        if (_bug32_ptr == NULL)
            trigger = 1;
        _bug32_ptr = NULL;
        pthread_mutex_unlock(&_bug32_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug32_mutex1);
        _bug32_ptr = &_bug32_val;
        if (_bug32_ptr == NULL)
            trigger = 1;
        _bug32_ptr = NULL;
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
    static const uint32_t indices[] = {0x269e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug32_input_helper.interleave_num = 0;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0xa1182c9e;
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
static volatile __attribute__((used)) int _bug33_first_part;
static volatile __attribute__((used)) int _bug33_released;
static void _bug33_pattern_clean(void)
{
    pthread_mutex_init(&_bug33_mutex1, NULL);
    _bug33_first_part = -1;
    _bug33_released = -1;
}
static __attribute__((optimize(0))) int _bug33_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug33_mutex1) == 0)
                break;
            if (_bug33_released == 0) {
                _bug33_first_part = -1;
                _bug33_released = -1;
                return 1;
            }
        }
        if (_bug33_first_part == -1)
            _bug33_first_part = part;
        else {
            _bug33_first_part = -1;
            while (_bug33_released == -1) {}
            _bug33_released = -1;
        }
        pthread_mutex_unlock(&_bug33_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug33_mutex1);
        if (_bug33_first_part == -1) {
            _bug33_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug33_mutex1);
                _bug33_released = 1;
            }
            else {
                _bug33_released = 0;
                while (_bug33_released != -1) {}
                pthread_mutex_unlock(&_bug33_mutex1);
                return 1;
            }
        }
        else {
            _bug33_first_part = -1;
            pthread_mutex_unlock(&_bug33_mutex1);
        }
    }
    return 0;
}

static void _bug33_pattern_clean(void);
static int _bug33_pattern(int part);
static racebench_bug_helper _bug33_bug_helper;
static racebench_input_helper _bug33_input_helper;
__attribute__((constructor))
static void _bug33_input_helper_init(void)
{
    static const uint32_t indices[] = {0x62};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x3e4ac621};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x74044b9b};
    static racebench_before_helper before_helpers[2] = {};
    _bug33_input_helper.interleave_num = 2;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0xb24f11bc;
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
    static const uint32_t indices[] = {0xc45};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x6910395a};
    static racebench_before_helper before_helpers[0] = {};
    _bug34_input_helper.interleave_num = 0;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0x6910395a;
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
static void _bug35_pattern_clean(void)
{
    pthread_mutex_init(&_bug35_mutex1, NULL);
    _bug35_first_part = -1;
}
static __attribute__((optimize(0))) int _bug35_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug35_mutex1);
        if (_bug35_first_part == -1)
            _bug35_first_part = part;
        else
            _bug35_first_part = -1;
        pthread_mutex_unlock(&_bug35_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug35_mutex1);
        if (_bug35_first_part == -1) {
            _bug35_first_part = part;
            trigger = 1;
        }
        else
            _bug35_first_part = -1;
        pthread_mutex_unlock(&_bug35_mutex1);
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
    static const uint32_t indices[] = {0x182d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1150e0e};
    static racebench_before_helper before_helpers[1] = {};
    _bug35_input_helper.interleave_num = 1;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xa1150e0e;
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
static pthread_cond_t _bug36_cond1;
static volatile __attribute__((used)) int* volatile _bug36_ptr;
static volatile __attribute__((used)) int _bug36_val;
static volatile __attribute__((used)) int _bug36_pred;
static void _bug36_pattern_clean(void)
{
    pthread_mutex_init(&_bug36_mutex1, NULL);
    pthread_cond_init(&_bug36_cond1, NULL);
    _bug36_ptr = NULL;
    _bug36_val = 0;
    _bug36_pred = 0;
}
static __attribute__((optimize(0))) int _bug36_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug36_pred = 0;
        pthread_cond_signal(&_bug36_cond1);
        pthread_mutex_lock(&_bug36_mutex1);
        _bug36_ptr = &_bug36_val;
        _bug36_pred = 1;
        pthread_mutex_unlock(&_bug36_mutex1);
        pthread_cond_signal(&_bug36_cond1);
    }
    else {
        pthread_mutex_lock(&_bug36_mutex1);
        if (!_bug36_pred)
            pthread_cond_wait(&_bug36_cond1, &_bug36_mutex1);
        if (_bug36_ptr == NULL)
            trigger = 1;
        _bug36_ptr = NULL;
        _bug36_pred = 0;
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
    static const uint32_t indices[] = {0x34b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x83b0fe61};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x3355cd19};
    static racebench_before_helper before_helpers[2] = {};
    _bug36_input_helper.interleave_num = 2;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0xb706cb7a;
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
static volatile __attribute__((used)) int _bug37_first_part;
static void _bug37_pattern_clean(void)
{
    pthread_mutex_init(&_bug37_mutex1, NULL);
    _bug37_first_part = -1;
}
static __attribute__((optimize(0))) int _bug37_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug37_mutex1);
        if (_bug37_first_part == -1)
            _bug37_first_part = part;
        else
            _bug37_first_part = -1;
        pthread_mutex_unlock(&_bug37_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug37_mutex1);
        if (_bug37_first_part == -1) {
            _bug37_first_part = part;
            trigger = 1;
        }
        else
            _bug37_first_part = -1;
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
    static const uint32_t indices[] = {0x542};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118303e};
    static racebench_before_helper before_helpers[0] = {};
    _bug37_input_helper.interleave_num = 0;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0xa118303e;
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
static volatile __attribute__((used)) int* volatile _bug38_ptr;
static volatile __attribute__((used)) int _bug38_val;
static volatile __attribute__((used)) int _bug38_pred;
static void _bug38_pattern_clean(void)
{
    pthread_mutex_init(&_bug38_mutex1, NULL);
    pthread_cond_init(&_bug38_cond1, NULL);
    _bug38_ptr = NULL;
    _bug38_val = 0;
    _bug38_pred = 0;
}
static __attribute__((optimize(0))) int _bug38_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug38_pred = 0;
        pthread_cond_signal(&_bug38_cond1);
        pthread_mutex_lock(&_bug38_mutex1);
        _bug38_ptr = &_bug38_val;
        _bug38_pred = 1;
        pthread_mutex_unlock(&_bug38_mutex1);
        pthread_cond_signal(&_bug38_cond1);
    }
    else {
        pthread_mutex_lock(&_bug38_mutex1);
        if (!_bug38_pred)
            pthread_cond_wait(&_bug38_cond1, &_bug38_mutex1);
        if (_bug38_ptr == NULL)
            trigger = 1;
        _bug38_ptr = NULL;
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
    static const uint32_t indices[] = {0x206b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xe72749fb};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xc4829c9f};
    static racebench_before_helper before_helpers[2] = {};
    _bug38_input_helper.interleave_num = 2;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xaba9e69a;
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
static volatile __attribute__((used)) int* volatile _bug39_ptr;
static volatile __attribute__((used)) int _bug39_val;
static void _bug39_pattern_clean(void)
{
    pthread_mutex_init(&_bug39_mutex1, NULL);
    _bug39_ptr = NULL;
    _bug39_val = 0;
}
static __attribute__((optimize(0))) int _bug39_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug39_mutex1);
        _bug39_ptr = &_bug39_val;
        pthread_mutex_unlock(&_bug39_mutex1);
        pthread_mutex_lock(&_bug39_mutex1);
        if (_bug39_ptr == NULL)
            trigger = 1;
        _bug39_ptr = NULL;
        pthread_mutex_unlock(&_bug39_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug39_mutex1);
        _bug39_ptr = &_bug39_val;
        if (_bug39_ptr == NULL)
            trigger = 1;
        _bug39_ptr = NULL;
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
    static const uint32_t indices[] = {0x6f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[0] = {};
    _bug39_input_helper.interleave_num = 0;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0xa1182c8e;
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
static volatile __attribute__((used)) int _bug40_first_part;
static volatile __attribute__((used)) int _bug40_released;
static void _bug40_pattern_clean(void)
{
    pthread_mutex_init(&_bug40_mutex1, NULL);
    _bug40_first_part = -1;
    _bug40_released = -1;
}
static __attribute__((optimize(0))) int _bug40_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug40_mutex1) == 0)
                break;
            if (_bug40_released == 0) {
                _bug40_first_part = -1;
                _bug40_released = -1;
                return 1;
            }
        }
        if (_bug40_first_part == -1)
            _bug40_first_part = part;
        else {
            _bug40_first_part = -1;
            while (_bug40_released == -1) {}
            _bug40_released = -1;
        }
        pthread_mutex_unlock(&_bug40_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug40_mutex1);
        if (_bug40_first_part == -1) {
            _bug40_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug40_mutex1);
                _bug40_released = 1;
            }
            else {
                _bug40_released = 0;
                while (_bug40_released != -1) {}
                pthread_mutex_unlock(&_bug40_mutex1);
                return 1;
            }
        }
        else {
            _bug40_first_part = -1;
            pthread_mutex_unlock(&_bug40_mutex1);
        }
    }
    return 0;
}

static void _bug40_pattern_clean(void);
static int _bug40_pattern(int part);
static racebench_bug_helper _bug40_bug_helper;
static racebench_input_helper _bug40_input_helper;
__attribute__((constructor))
static void _bug40_input_helper_init(void)
{
    static const uint32_t indices[] = {0x16ec};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x7adf40b8, 0xbc02800a};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa118276e};
    static racebench_before_helper before_helpers[3] = {};
    _bug40_input_helper.interleave_num = 3;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xd7f9e830;
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
static volatile __attribute__((used)) int _bug41_val;
static void _bug41_pattern_clean(void)
{
    pthread_mutex_init(&_bug41_mutex1, NULL);
    _bug41_val = 0;
}
static __attribute__((optimize(0))) int _bug41_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug41_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug41_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug41_mutex1);
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

static void _bug41_pattern_clean(void);
static int _bug41_pattern(int part);
static racebench_bug_helper _bug41_bug_helper;
static racebench_input_helper _bug41_input_helper;
__attribute__((constructor))
static void _bug41_input_helper_init(void)
{
    static const uint32_t indices[] = {0x19ae};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1127cfe};
    static racebench_before_helper before_helpers[1] = {};
    _bug41_input_helper.interleave_num = 1;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa1127cfe;
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
static volatile __attribute__((used)) int _bug42_first_part;
static volatile __attribute__((used)) int _bug42_released;
static void _bug42_pattern_clean(void)
{
    pthread_mutex_init(&_bug42_mutex1, NULL);
    _bug42_first_part = -1;
    _bug42_released = -1;
}
static __attribute__((optimize(0))) int _bug42_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug42_mutex1) == 0)
                break;
            if (_bug42_released == 0) {
                _bug42_first_part = -1;
                _bug42_released = -1;
                return 1;
            }
        }
        if (_bug42_first_part == -1)
            _bug42_first_part = part;
        else {
            _bug42_first_part = -1;
            while (_bug42_released == -1) {}
            _bug42_released = -1;
        }
        pthread_mutex_unlock(&_bug42_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug42_mutex1);
        if (_bug42_first_part == -1) {
            _bug42_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug42_mutex1);
                _bug42_released = 1;
            }
            else {
                _bug42_released = 0;
                while (_bug42_released != -1) {}
                pthread_mutex_unlock(&_bug42_mutex1);
                return 1;
            }
        }
        else {
            _bug42_first_part = -1;
            pthread_mutex_unlock(&_bug42_mutex1);
        }
    }
    return 0;
}

static void _bug42_pattern_clean(void);
static int _bug42_pattern(int part);
static racebench_bug_helper _bug42_bug_helper;
static racebench_input_helper _bug42_input_helper;
__attribute__((constructor))
static void _bug42_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1c48};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x39b56671};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x2661303f};
    static racebench_before_helper before_helpers[2] = {};
    _bug42_input_helper.interleave_num = 2;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0x601696b0;
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
static pthread_mutex_t _bug43_mutex2;
static void _bug43_pattern_clean(void)
{
    pthread_mutex_init(&_bug43_mutex1, NULL);
    pthread_mutex_init(&_bug43_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug43_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug43_mutex1);
        if (pthread_mutex_trylock(&_bug43_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug43_mutex2);
        pthread_mutex_unlock(&_bug43_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug43_mutex2);
        if (pthread_mutex_trylock(&_bug43_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug43_mutex1);
        pthread_mutex_unlock(&_bug43_mutex2);
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
    static const uint32_t indices[] = {0xc72};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1168f6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug43_input_helper.interleave_num = 0;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0xa1168f6e;
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
static volatile __attribute__((used)) int _bug44_ok;
static volatile __attribute__((used)) int* volatile _bug44_ptr;
static volatile __attribute__((used)) int _bug44_val;
static void _bug44_pattern_clean(void)
{
    _bug44_ok = 0;
    _bug44_ptr = NULL;
    _bug44_val = 0;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug44_ok = 1;
        __sync_synchronize();
        _bug44_ptr = &_bug44_val;
    }
    else {
        while (!_bug44_ok)
            pthread_yield();
        if (_bug44_ptr == NULL)
            trigger = 1;
        _bug44_ptr = NULL;
        _bug44_ok = 0;
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
    static const uint32_t indices[] = {0x8f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11e8aae};
    static racebench_before_helper before_helpers[1] = {};
    _bug44_input_helper.interleave_num = 1;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xa11e8aae;
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
static volatile __attribute__((used)) int _bug45_val;
static void _bug45_pattern_clean(void)
{
    pthread_mutex_init(&_bug45_mutex1, NULL);
    _bug45_val = 0;
}
static __attribute__((optimize(0))) int _bug45_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug45_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug45_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug45_mutex1);
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

static void _bug45_pattern_clean(void);
static int _bug45_pattern(int part);
static racebench_bug_helper _bug45_bug_helper;
static racebench_input_helper _bug45_input_helper;
__attribute__((constructor))
static void _bug45_input_helper_init(void)
{
    static const uint32_t indices[] = {0xa6e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xc959b361};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x7374fcb0};
    static racebench_before_helper before_helpers[2] = {};
    _bug45_input_helper.interleave_num = 2;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0x3cceb011;
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
    static const uint32_t indices[] = {0xaa};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1166c9e};
    static racebench_before_helper before_helpers[0] = {};
    _bug46_input_helper.interleave_num = 0;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0xa1166c9e;
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
static volatile __attribute__((used)) int _bug47_ok;
static volatile __attribute__((used)) int* volatile _bug47_ptr;
static volatile __attribute__((used)) int _bug47_val;
static void _bug47_pattern_clean(void)
{
    _bug47_ok = 0;
    _bug47_ptr = NULL;
    _bug47_val = 0;
}
static __attribute__((optimize(0))) int _bug47_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug47_ok = 1;
        __sync_synchronize();
        _bug47_ptr = &_bug47_val;
    }
    else {
        while (!_bug47_ok)
            pthread_yield();
        if (_bug47_ptr == NULL)
            trigger = 1;
        _bug47_ptr = NULL;
        _bug47_ok = 0;
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
    static const uint32_t indices[] = {0x1441};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x7969555e};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182bbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug47_input_helper.interleave_num = 2;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0x1a81811c;
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
static volatile __attribute__((used)) int _bug48_val;
static void _bug48_pattern_clean(void)
{
    pthread_mutex_init(&_bug48_mutex1, NULL);
    _bug48_val = 0;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug48_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug48_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug48_mutex1);
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

static void _bug48_pattern_clean(void);
static int _bug48_pattern(int part);
static racebench_bug_helper _bug48_bug_helper;
static racebench_input_helper _bug48_input_helper;
__attribute__((constructor))
static void _bug48_input_helper_init(void)
{
    static const uint32_t indices[] = {0x33a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xcc74a8c6};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x28952acf};
    static racebench_before_helper before_helpers[2] = {};
    _bug48_input_helper.interleave_num = 2;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0xf509d395;
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
    static const uint32_t indices[] = {0xb20};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118310e};
    static racebench_before_helper before_helpers[0] = {};
    _bug49_input_helper.interleave_num = 0;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xa118310e;
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
static volatile __attribute__((used)) int _bug50_ok;
static volatile __attribute__((used)) int* volatile _bug50_ptr;
static volatile __attribute__((used)) int _bug50_val;
static void _bug50_pattern_clean(void)
{
    _bug50_ok = 0;
    _bug50_ptr = NULL;
    _bug50_val = 0;
}
static __attribute__((optimize(0))) int _bug50_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug50_ok = 1;
        __sync_synchronize();
        _bug50_ptr = &_bug50_val;
    }
    else {
        while (!_bug50_ok)
            pthread_yield();
        if (_bug50_ptr == NULL)
            trigger = 1;
        _bug50_ptr = NULL;
        _bug50_ok = 0;
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
    static const uint32_t indices[] = {0x1e73};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2ad97079};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1115f5e};
    static racebench_before_helper before_helpers[2] = {};
    _bug50_input_helper.interleave_num = 2;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0xcbeacfd7;
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
static volatile __attribute__((used)) int _bug51_arr[3];
static volatile __attribute__((used)) int _bug51_val;
static volatile __attribute__((used)) int _bug51_pred;
static void _bug51_pattern_clean(void)
{
    _bug51_arr[0] = 0, _bug51_arr[1] = 2, _bug51_arr[2] = 1;
    _bug51_val = 0;
    _bug51_pred = 0;
}
static __attribute__((optimize(0))) int _bug51_pattern(int part)
{
    int trigger = 0;
    _bug51_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug51_val += _bug51_arr[i];
        }
        __sync_synchronize();
        while (!_bug51_pred)
            pthread_yield();
        trigger = _bug51_val != 3;
        _bug51_pred = 0;
        _bug51_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug51_val += _bug51_arr[i];
        }
        __sync_synchronize();
        _bug51_pred = 1;
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
    static const uint32_t indices[] = {0x1632};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x6cd8de6b};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1c5701c};
    static racebench_before_helper before_helpers[2] = {};
    _bug51_input_helper.interleave_num = 2;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0xe9e4e87;
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
    static const uint32_t indices[] = {0xff9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb6c806e};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x2f9470c0};
    static racebench_before_helper before_helpers[2] = {};
    _bug52_input_helper.interleave_num = 2;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0x3b00f12e;
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
static volatile __attribute__((used)) int* volatile _bug53_ptr;
static volatile __attribute__((used)) int _bug53_val;
static volatile __attribute__((used)) int _bug53_pred;
static void _bug53_pattern_clean(void)
{
    pthread_mutex_init(&_bug53_mutex1, NULL);
    pthread_cond_init(&_bug53_cond1, NULL);
    _bug53_ptr = NULL;
    _bug53_val = 0;
    _bug53_pred = 0;
}
static __attribute__((optimize(0))) int _bug53_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug53_pred = 0;
        pthread_cond_signal(&_bug53_cond1);
        pthread_mutex_lock(&_bug53_mutex1);
        _bug53_ptr = &_bug53_val;
        _bug53_pred = 1;
        pthread_mutex_unlock(&_bug53_mutex1);
        pthread_cond_signal(&_bug53_cond1);
    }
    else {
        pthread_mutex_lock(&_bug53_mutex1);
        if (!_bug53_pred)
            pthread_cond_wait(&_bug53_cond1, &_bug53_mutex1);
        if (_bug53_ptr == NULL)
            trigger = 1;
        _bug53_ptr = NULL;
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
    static const uint32_t indices[] = {0x1ce8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7cc7bef5};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11f58ce};
    static racebench_before_helper before_helpers[2] = {};
    _bug53_input_helper.interleave_num = 2;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0x1de717c3;
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
static volatile __attribute__((used)) int _bug54_ok;
static volatile __attribute__((used)) int* volatile _bug54_ptr;
static volatile __attribute__((used)) int _bug54_val;
static void _bug54_pattern_clean(void)
{
    _bug54_ok = 0;
    _bug54_ptr = NULL;
    _bug54_val = 0;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug54_ok = 1;
        __sync_synchronize();
        _bug54_ptr = &_bug54_val;
    }
    else {
        while (!_bug54_ok)
            pthread_yield();
        if (_bug54_ptr == NULL)
            trigger = 1;
        _bug54_ptr = NULL;
        _bug54_ok = 0;
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
    static const uint32_t indices[] = {0x513};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug54_input_helper.interleave_num = 0;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0xa1182c4e;
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
static volatile __attribute__((used)) int _bug55_released;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    _bug55_first_part = -1;
    _bug55_released = -1;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug55_mutex1) == 0)
                break;
            if (_bug55_released == 0) {
                _bug55_first_part = -1;
                _bug55_released = -1;
                return 1;
            }
        }
        if (_bug55_first_part == -1)
            _bug55_first_part = part;
        else {
            _bug55_first_part = -1;
            while (_bug55_released == -1) {}
            _bug55_released = -1;
        }
        pthread_mutex_unlock(&_bug55_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug55_mutex1);
        if (_bug55_first_part == -1) {
            _bug55_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug55_mutex1);
                _bug55_released = 1;
            }
            else {
                _bug55_released = 0;
                while (_bug55_released != -1) {}
                pthread_mutex_unlock(&_bug55_mutex1);
                return 1;
            }
        }
        else {
            _bug55_first_part = -1;
            pthread_mutex_unlock(&_bug55_mutex1);
        }
    }
    return 0;
}

static void _bug55_pattern_clean(void);
static int _bug55_pattern(int part);
static racebench_bug_helper _bug55_bug_helper;
static racebench_input_helper _bug55_input_helper;
__attribute__((constructor))
static void _bug55_input_helper_init(void)
{
    static const uint32_t indices[] = {0x24f1};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa9055ffe};
    static racebench_before_helper before_helpers[1] = {};
    _bug55_input_helper.interleave_num = 1;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0xa9055ffe;
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
    static const uint32_t indices[] = {0x264};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x66f22fef};
    static racebench_before_helper before_helpers[0] = {};
    _bug56_input_helper.interleave_num = 0;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0x66f22fef;
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
static volatile __attribute__((used)) int _bug57_released;
static void _bug57_pattern_clean(void)
{
    pthread_mutex_init(&_bug57_mutex1, NULL);
    _bug57_first_part = -1;
    _bug57_released = -1;
}
static __attribute__((optimize(0))) int _bug57_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug57_mutex1) == 0)
                break;
            if (_bug57_released == 0) {
                _bug57_first_part = -1;
                _bug57_released = -1;
                return 1;
            }
        }
        if (_bug57_first_part == -1)
            _bug57_first_part = part;
        else {
            _bug57_first_part = -1;
            while (_bug57_released == -1) {}
            _bug57_released = -1;
        }
        pthread_mutex_unlock(&_bug57_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug57_mutex1);
        if (_bug57_first_part == -1) {
            _bug57_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug57_mutex1);
                _bug57_released = 1;
            }
            else {
                _bug57_released = 0;
                while (_bug57_released != -1) {}
                pthread_mutex_unlock(&_bug57_mutex1);
                return 1;
            }
        }
        else {
            _bug57_first_part = -1;
            pthread_mutex_unlock(&_bug57_mutex1);
        }
    }
    return 0;
}

static void _bug57_pattern_clean(void);
static int _bug57_pattern(int part);
static racebench_bug_helper _bug57_bug_helper;
static racebench_input_helper _bug57_input_helper;
__attribute__((constructor))
static void _bug57_input_helper_init(void)
{
    static const uint32_t indices[] = {0xcfc};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x85042a1f};
    static racebench_before_helper before_helpers[0] = {};
    _bug57_input_helper.interleave_num = 0;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0x85042a1f;
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
    static const uint32_t indices[] = {0x1ad9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xaaedbc30, 0xfaaffad};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xb425795b};
    static racebench_before_helper before_helpers[3] = {};
    _bug58_input_helper.interleave_num = 3;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0x6ebe3538;
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
    static const uint32_t indices[] = {0xbf0};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa111d00e};
    static racebench_before_helper before_helpers[1] = {};
    _bug59_input_helper.interleave_num = 1;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0xa111d00e;
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
    static const uint32_t indices[] = {0x75b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x137cb87e, 0xafb741e6};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115ac1e};
    static racebench_before_helper before_helpers[3] = {};
    _bug60_input_helper.interleave_num = 3;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0x6449a682;
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
    static const uint32_t indices[] = {0x16b3};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1209c5e};
    static racebench_before_helper before_helpers[1] = {};
    _bug61_input_helper.interleave_num = 1;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0xa1209c5e;
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
    static const uint32_t indices[] = {0x153f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1155c9e};
    static racebench_before_helper before_helpers[1] = {};
    _bug62_input_helper.interleave_num = 1;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0xa1155c9e;
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
static pthread_mutex_t _bug63_mutex2;
static void _bug63_pattern_clean(void)
{
    pthread_mutex_init(&_bug63_mutex1, NULL);
    pthread_mutex_init(&_bug63_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug63_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug63_mutex1);
        if (pthread_mutex_trylock(&_bug63_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug63_mutex2);
        pthread_mutex_unlock(&_bug63_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug63_mutex2);
        if (pthread_mutex_trylock(&_bug63_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug63_mutex1);
        pthread_mutex_unlock(&_bug63_mutex2);
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
    static const uint32_t indices[] = {0x20c1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xc0ff0b5c, 0x28015a9f};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11830de};
    static racebench_before_helper before_helpers[3] = {};
    _bug63_input_helper.interleave_num = 3;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x8a1896d9;
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
    static const uint32_t indices[] = {0x2082};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x32374e15, 0xe72fc8b3};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa111104e};
    static racebench_before_helper before_helpers[3] = {};
    _bug64_input_helper.interleave_num = 3;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0xba782716;
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
static volatile __attribute__((used)) int _bug65_first_part;
static volatile __attribute__((used)) int _bug65_released;
static void _bug65_pattern_clean(void)
{
    pthread_mutex_init(&_bug65_mutex1, NULL);
    _bug65_first_part = -1;
    _bug65_released = -1;
}
static __attribute__((optimize(0))) int _bug65_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug65_mutex1) == 0)
                break;
            if (_bug65_released == 0) {
                _bug65_first_part = -1;
                _bug65_released = -1;
                return 1;
            }
        }
        if (_bug65_first_part == -1)
            _bug65_first_part = part;
        else {
            _bug65_first_part = -1;
            while (_bug65_released == -1) {}
            _bug65_released = -1;
        }
        pthread_mutex_unlock(&_bug65_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug65_mutex1);
        if (_bug65_first_part == -1) {
            _bug65_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug65_mutex1);
                _bug65_released = 1;
            }
            else {
                _bug65_released = 0;
                while (_bug65_released != -1) {}
                pthread_mutex_unlock(&_bug65_mutex1);
                return 1;
            }
        }
        else {
            _bug65_first_part = -1;
            pthread_mutex_unlock(&_bug65_mutex1);
        }
    }
    return 0;
}

static void _bug65_pattern_clean(void);
static int _bug65_pattern(int part);
static racebench_bug_helper _bug65_bug_helper;
static racebench_input_helper _bug65_input_helper;
__attribute__((constructor))
static void _bug65_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1f3d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xc386361d};
    static racebench_before_helper before_helpers[1] = {};
    _bug65_input_helper.interleave_num = 1;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0xc386361d;
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
static pthread_mutex_t _bug66_mutex2;
static void _bug66_pattern_clean(void)
{
    pthread_mutex_init(&_bug66_mutex1, NULL);
    pthread_mutex_init(&_bug66_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug66_mutex1);
        if (pthread_mutex_trylock(&_bug66_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug66_mutex2);
        pthread_mutex_unlock(&_bug66_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug66_mutex2);
        if (pthread_mutex_trylock(&_bug66_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug66_mutex1);
        pthread_mutex_unlock(&_bug66_mutex2);
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
    static const uint32_t indices[] = {0xa3d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xd80d6ec, 0x43027c1e};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x3b0370f};
    static racebench_before_helper before_helpers[3] = {};
    _bug66_input_helper.interleave_num = 3;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0x54338a19;
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
static volatile __attribute__((used)) int _bug67_first_part;
static void _bug67_pattern_clean(void)
{
    pthread_mutex_init(&_bug67_mutex1, NULL);
    _bug67_first_part = -1;
}
static __attribute__((optimize(0))) int _bug67_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug67_mutex1);
        if (_bug67_first_part == -1)
            _bug67_first_part = part;
        else
            _bug67_first_part = -1;
        pthread_mutex_unlock(&_bug67_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug67_mutex1);
        if (_bug67_first_part == -1) {
            _bug67_first_part = part;
            trigger = 1;
        }
        else
            _bug67_first_part = -1;
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
    static const uint32_t indices[] = {0x1e63};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xeb44c917, 0xcf891b6};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug67_input_helper.interleave_num = 3;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0x9955874b;
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
static volatile __attribute__((used)) int _bug68_val;
static void _bug68_pattern_clean(void)
{
    pthread_mutex_init(&_bug68_mutex1, NULL);
    _bug68_val = 0;
}
static __attribute__((optimize(0))) int _bug68_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug68_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug68_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug68_mutex1);
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

static void _bug68_pattern_clean(void);
static int _bug68_pattern(int part);
static racebench_bug_helper _bug68_bug_helper;
static racebench_input_helper _bug68_input_helper;
__attribute__((constructor))
static void _bug68_input_helper_init(void)
{
    static const uint32_t indices[] = {0x15d2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug68_input_helper.interleave_num = 1;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0xa1182f4e;
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
    static const uint32_t indices[] = {0x84f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x9967b7fc};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa114811e};
    static racebench_before_helper before_helpers[2] = {};
    _bug69_input_helper.interleave_num = 2;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0x3a7c391a;
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
    static const uint32_t indices[] = {0x198c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x971dd202};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182eee};
    static racebench_before_helper before_helpers[2] = {};
    _bug70_input_helper.interleave_num = 2;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0x383600f0;
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
    static const uint32_t indices[] = {0x1a99};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xbeefb3b0};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa114689e};
    static racebench_before_helper before_helpers[2] = {};
    _bug71_input_helper.interleave_num = 2;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0x60041c4e;
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
static volatile __attribute__((used)) int _bug72_pred;
static volatile __attribute__((used)) int _bug72_sent;
static void _bug72_pattern_clean(void)
{
    pthread_mutex_init(&_bug72_mutex1, NULL);
    pthread_cond_init(&_bug72_cond1, NULL);
    _bug72_pred = 0;
    _bug72_sent = 0;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug72_mutex1);
        _bug72_pred = 1;
        pthread_cond_signal(&_bug72_cond1);
        _bug72_sent = 1;
        pthread_mutex_unlock(&_bug72_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug72_mutex1);
        if (_bug72_sent) {
            trigger = 1;
        }
        else {
            while (_bug72_pred == 0)
                pthread_cond_wait(&_bug72_cond1, &_bug72_mutex1);
        }
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
    static const uint32_t indices[] = {0x17a8};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x3ab178e1};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f0e};
    static racebench_before_helper before_helpers[2] = {};
    _bug72_input_helper.interleave_num = 2;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xdbc9a7ef;
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
static volatile __attribute__((used)) int _bug73_first_part;
static void _bug73_pattern_clean(void)
{
    pthread_mutex_init(&_bug73_mutex1, NULL);
    _bug73_first_part = -1;
}
static __attribute__((optimize(0))) int _bug73_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug73_mutex1);
        if (_bug73_first_part == -1)
            _bug73_first_part = part;
        else
            _bug73_first_part = -1;
        pthread_mutex_unlock(&_bug73_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug73_mutex1);
        if (_bug73_first_part == -1) {
            _bug73_first_part = part;
            trigger = 1;
        }
        else
            _bug73_first_part = -1;
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
    static const uint32_t indices[] = {0x1aa2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa118308e};
    static racebench_before_helper before_helpers[1] = {};
    _bug73_input_helper.interleave_num = 1;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0xa118308e;
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
static volatile __attribute__((used)) int _bug74_ok;
static volatile __attribute__((used)) int* volatile _bug74_ptr;
static volatile __attribute__((used)) int _bug74_val;
static void _bug74_pattern_clean(void)
{
    _bug74_ok = 0;
    _bug74_ptr = NULL;
    _bug74_val = 0;
}
static __attribute__((optimize(0))) int _bug74_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug74_ok = 1;
        __sync_synchronize();
        _bug74_ptr = &_bug74_val;
    }
    else {
        while (!_bug74_ok)
            pthread_yield();
        if (_bug74_ptr == NULL)
            trigger = 1;
        _bug74_ptr = NULL;
        _bug74_ok = 0;
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
    static const uint32_t indices[] = {0x918};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182b1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug74_input_helper.interleave_num = 1;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0xa1182b1e;
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
static pthread_cond_t _bug75_cond1;
static volatile __attribute__((used)) int* volatile _bug75_ptr;
static volatile __attribute__((used)) int _bug75_val;
static volatile __attribute__((used)) int _bug75_pred;
static void _bug75_pattern_clean(void)
{
    pthread_mutex_init(&_bug75_mutex1, NULL);
    pthread_cond_init(&_bug75_cond1, NULL);
    _bug75_ptr = NULL;
    _bug75_val = 0;
    _bug75_pred = 0;
}
static __attribute__((optimize(0))) int _bug75_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug75_pred = 0;
        pthread_cond_signal(&_bug75_cond1);
        pthread_mutex_lock(&_bug75_mutex1);
        _bug75_ptr = &_bug75_val;
        _bug75_pred = 1;
        pthread_mutex_unlock(&_bug75_mutex1);
        pthread_cond_signal(&_bug75_cond1);
    }
    else {
        pthread_mutex_lock(&_bug75_mutex1);
        if (!_bug75_pred)
            pthread_cond_wait(&_bug75_cond1, &_bug75_mutex1);
        if (_bug75_ptr == NULL)
            trigger = 1;
        _bug75_ptr = NULL;
        _bug75_pred = 0;
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
    static const uint32_t indices[] = {0x1694};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112ecde};
    static racebench_before_helper before_helpers[0] = {};
    _bug75_input_helper.interleave_num = 0;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xa112ecde;
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
static volatile __attribute__((used)) int _bug76_val;
static void _bug76_pattern_clean(void)
{
    pthread_mutex_init(&_bug76_mutex1, NULL);
    _bug76_val = 0;
}
static __attribute__((optimize(0))) int _bug76_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug76_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug76_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug76_mutex1);
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

static void _bug76_pattern_clean(void);
static int _bug76_pattern(int part);
static racebench_bug_helper _bug76_bug_helper;
static racebench_input_helper _bug76_input_helper;
__attribute__((constructor))
static void _bug76_input_helper_init(void)
{
    static const uint32_t indices[] = {0x17a2};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x61ab2953};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1151a8e};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0x2c043e1;
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
    static const uint32_t indices[] = {0x262b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x88e5a676};
    static racebench_before_helper before_helpers[0] = {};
    _bug77_input_helper.interleave_num = 0;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0x88e5a676;
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
    static const uint32_t indices[] = {0x219b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x13b55ae2};
    static racebench_before_helper before_helpers[0] = {};
    _bug78_input_helper.interleave_num = 0;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0x13b55ae2;
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
    static const uint32_t indices[] = {0x210e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x2a994633, 0x5c11b090};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[3] = {};
    _bug79_input_helper.interleave_num = 3;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0x27c32351;
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
static pthread_cond_t _bug80_cond1;
static volatile __attribute__((used)) int* volatile _bug80_ptr;
static volatile __attribute__((used)) int _bug80_val;
static volatile __attribute__((used)) int _bug80_pred;
static void _bug80_pattern_clean(void)
{
    pthread_mutex_init(&_bug80_mutex1, NULL);
    pthread_cond_init(&_bug80_cond1, NULL);
    _bug80_ptr = NULL;
    _bug80_val = 0;
    _bug80_pred = 0;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug80_pred = 0;
        pthread_cond_signal(&_bug80_cond1);
        pthread_mutex_lock(&_bug80_mutex1);
        _bug80_ptr = &_bug80_val;
        _bug80_pred = 1;
        pthread_mutex_unlock(&_bug80_mutex1);
        pthread_cond_signal(&_bug80_cond1);
    }
    else {
        pthread_mutex_lock(&_bug80_mutex1);
        if (!_bug80_pred)
            pthread_cond_wait(&_bug80_cond1, &_bug80_mutex1);
        if (_bug80_ptr == NULL)
            trigger = 1;
        _bug80_ptr = NULL;
        _bug80_pred = 0;
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
    static const uint32_t indices[] = {0x1eb9};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa11a5cce};
    static racebench_before_helper before_helpers[1] = {};
    _bug80_input_helper.interleave_num = 1;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xa11a5cce;
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
static volatile __attribute__((used)) int _bug81_pred;
static volatile __attribute__((used)) int _bug81_sent;
static void _bug81_pattern_clean(void)
{
    pthread_mutex_init(&_bug81_mutex1, NULL);
    pthread_cond_init(&_bug81_cond1, NULL);
    _bug81_pred = 0;
    _bug81_sent = 0;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug81_mutex1);
        _bug81_pred = 1;
        pthread_cond_signal(&_bug81_cond1);
        _bug81_sent = 1;
        pthread_mutex_unlock(&_bug81_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug81_mutex1);
        if (_bug81_sent) {
            trigger = 1;
        }
        else {
            while (_bug81_pred == 0)
                pthread_cond_wait(&_bug81_cond1, &_bug81_mutex1);
        }
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
    static const uint32_t indices[] = {0x1abf};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118309e};
    static racebench_before_helper before_helpers[0] = {};
    _bug81_input_helper.interleave_num = 0;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xa118309e;
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
    static const uint32_t indices[] = {0x1249};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xfdb8cfd4, 0xafa89c00};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x94250475};
    static racebench_before_helper before_helpers[3] = {};
    _bug82_input_helper.interleave_num = 3;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0x41867049;
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
    static const uint32_t indices[] = {0xbbd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xbdc68b98};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1160c1e};
    static racebench_before_helper before_helpers[2] = {};
    _bug83_input_helper.interleave_num = 2;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0x5edc97b6;
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
static volatile __attribute__((used)) int* volatile _bug84_ptr;
static volatile __attribute__((used)) int _bug84_val;
static void _bug84_pattern_clean(void)
{
    pthread_mutex_init(&_bug84_mutex1, NULL);
    _bug84_ptr = NULL;
    _bug84_val = 0;
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug84_mutex1);
        _bug84_ptr = &_bug84_val;
        pthread_mutex_unlock(&_bug84_mutex1);
        pthread_mutex_lock(&_bug84_mutex1);
        if (_bug84_ptr == NULL)
            trigger = 1;
        _bug84_ptr = NULL;
        pthread_mutex_unlock(&_bug84_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug84_mutex1);
        _bug84_ptr = &_bug84_val;
        if (_bug84_ptr == NULL)
            trigger = 1;
        _bug84_ptr = NULL;
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
    static const uint32_t indices[] = {0x2781};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[1] = {};
    _bug84_input_helper.interleave_num = 1;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xa1182cce;
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
static volatile __attribute__((used)) int _bug85_val;
static void _bug85_pattern_clean(void)
{
    pthread_mutex_init(&_bug85_mutex1, NULL);
    _bug85_val = 0;
}
static __attribute__((optimize(0))) int _bug85_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug85_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug85_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug85_mutex1);
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

static void _bug85_pattern_clean(void);
static int _bug85_pattern(int part);
static racebench_bug_helper _bug85_bug_helper;
static racebench_input_helper _bug85_input_helper;
__attribute__((constructor))
static void _bug85_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1262};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xc455fca1};
    static racebench_before_helper before_helpers[1] = {};
    _bug85_input_helper.interleave_num = 1;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0xc455fca1;
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
    static const uint32_t indices[] = {0x2159};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb30c2aa9};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c8e};
    static racebench_before_helper before_helpers[2] = {};
    _bug86_input_helper.interleave_num = 2;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x54245737;
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
static volatile __attribute__((used)) int _bug87_first_part;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    _bug87_first_part = -1;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug87_mutex1);
        if (_bug87_first_part == -1)
            _bug87_first_part = part;
        else
            _bug87_first_part = -1;
        pthread_mutex_unlock(&_bug87_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug87_mutex1);
        if (_bug87_first_part == -1) {
            _bug87_first_part = part;
            trigger = 1;
        }
        else
            _bug87_first_part = -1;
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
    static const uint32_t indices[] = {0x2323};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xe767973d, 0x94ffe786};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa117cc7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug87_input_helper.interleave_num = 3;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0x1d7f4b41;
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
    static const uint32_t indices[] = {0x252b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xafcdd643};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa120acde};
    static racebench_before_helper before_helpers[2] = {};
    _bug88_input_helper.interleave_num = 2;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0x50ee8321;
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
static volatile __attribute__((used)) int _bug89_pred;
static volatile __attribute__((used)) int _bug89_sent;
static void _bug89_pattern_clean(void)
{
    pthread_mutex_init(&_bug89_mutex1, NULL);
    pthread_cond_init(&_bug89_cond1, NULL);
    _bug89_pred = 0;
    _bug89_sent = 0;
}
static __attribute__((optimize(0))) int _bug89_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug89_mutex1);
        _bug89_pred = 1;
        pthread_cond_signal(&_bug89_cond1);
        _bug89_sent = 1;
        pthread_mutex_unlock(&_bug89_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug89_mutex1);
        if (_bug89_sent) {
            trigger = 1;
        }
        else {
            while (_bug89_pred == 0)
                pthread_cond_wait(&_bug89_cond1, &_bug89_mutex1);
        }
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
    static const uint32_t indices[] = {0xb62};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112895e};
    static racebench_before_helper before_helpers[0] = {};
    _bug89_input_helper.interleave_num = 0;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0xa112895e;
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
static pthread_cond_t _bug90_cond1;
static volatile __attribute__((used)) int* volatile _bug90_ptr;
static volatile __attribute__((used)) int _bug90_val;
static volatile __attribute__((used)) int _bug90_pred;
static void _bug90_pattern_clean(void)
{
    pthread_mutex_init(&_bug90_mutex1, NULL);
    pthread_cond_init(&_bug90_cond1, NULL);
    _bug90_ptr = NULL;
    _bug90_val = 0;
    _bug90_pred = 0;
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug90_pred = 0;
        pthread_cond_signal(&_bug90_cond1);
        pthread_mutex_lock(&_bug90_mutex1);
        _bug90_ptr = &_bug90_val;
        _bug90_pred = 1;
        pthread_mutex_unlock(&_bug90_mutex1);
        pthread_cond_signal(&_bug90_cond1);
    }
    else {
        pthread_mutex_lock(&_bug90_mutex1);
        if (!_bug90_pred)
            pthread_cond_wait(&_bug90_cond1, &_bug90_mutex1);
        if (_bug90_ptr == NULL)
            trigger = 1;
        _bug90_ptr = NULL;
        _bug90_pred = 0;
        pthread_mutex_unlock(&_bug90_mutex1);
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
    static const uint32_t indices[] = {0x1d3a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c2e};
    static racebench_before_helper before_helpers[1] = {};
    _bug90_input_helper.interleave_num = 1;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0xa1182c2e;
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
static volatile __attribute__((used)) int _bug91_arr[3];
static volatile __attribute__((used)) int _bug91_val;
static volatile __attribute__((used)) int _bug91_pred;
static void _bug91_pattern_clean(void)
{
    _bug91_arr[0] = 0, _bug91_arr[1] = 2, _bug91_arr[2] = 1;
    _bug91_val = 0;
    _bug91_pred = 0;
}
static __attribute__((optimize(0))) int _bug91_pattern(int part)
{
    int trigger = 0;
    _bug91_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug91_val += _bug91_arr[i];
        }
        __sync_synchronize();
        while (!_bug91_pred)
            pthread_yield();
        trigger = _bug91_val != 3;
        _bug91_pred = 0;
        _bug91_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug91_val += _bug91_arr[i];
        }
        __sync_synchronize();
        _bug91_pred = 1;
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
    static const uint32_t indices[] = {0x18ba};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x96f6ae58};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e9e};
    static racebench_before_helper before_helpers[2] = {};
    _bug91_input_helper.interleave_num = 2;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0x380edcf6;
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
static volatile __attribute__((used)) int _bug92_arr[3];
static volatile __attribute__((used)) int _bug92_val;
static volatile __attribute__((used)) int _bug92_pred;
static void _bug92_pattern_clean(void)
{
    _bug92_arr[0] = 0, _bug92_arr[1] = 2, _bug92_arr[2] = 1;
    _bug92_val = 0;
    _bug92_pred = 0;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    int trigger = 0;
    _bug92_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug92_val += _bug92_arr[i];
        }
        __sync_synchronize();
        while (!_bug92_pred)
            pthread_yield();
        trigger = _bug92_val != 3;
        _bug92_pred = 0;
        _bug92_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug92_val += _bug92_arr[i];
        }
        __sync_synchronize();
        _bug92_pred = 1;
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
    static const uint32_t indices[] = {0x263b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x80310212, 0x33780731};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa114297e};
    static racebench_before_helper before_helpers[3] = {};
    _bug92_input_helper.interleave_num = 3;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0x54bd32c1;
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
    static const uint32_t indices[] = {0x12f7};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x4458c100};
    static racebench_before_helper before_helpers[1] = {};
    _bug93_input_helper.interleave_num = 1;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0x4458c100;
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
    static const uint32_t indices[] = {0x16b9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x838493f4};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xf4951cd9};
    static racebench_before_helper before_helpers[2] = {};
    _bug94_input_helper.interleave_num = 2;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0x7819b0cd;
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
static volatile __attribute__((used)) int* volatile _bug95_ptr;
static volatile __attribute__((used)) int _bug95_val;
static void _bug95_pattern_clean(void)
{
    pthread_mutex_init(&_bug95_mutex1, NULL);
    _bug95_ptr = NULL;
    _bug95_val = 0;
}
static __attribute__((optimize(0))) int _bug95_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug95_mutex1);
        _bug95_ptr = &_bug95_val;
        pthread_mutex_unlock(&_bug95_mutex1);
        pthread_mutex_lock(&_bug95_mutex1);
        if (_bug95_ptr == NULL)
            trigger = 1;
        _bug95_ptr = NULL;
        pthread_mutex_unlock(&_bug95_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug95_mutex1);
        _bug95_ptr = &_bug95_val;
        if (_bug95_ptr == NULL)
            trigger = 1;
        _bug95_ptr = NULL;
        pthread_mutex_unlock(&_bug95_mutex1);
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
    static const uint32_t indices[] = {0x411};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1151c4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug95_input_helper.interleave_num = 1;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xa1151c4e;
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
static volatile __attribute__((used)) int* volatile _bug96_ptr;
static volatile __attribute__((used)) int _bug96_val;
static volatile __attribute__((used)) int _bug96_pred;
static void _bug96_pattern_clean(void)
{
    pthread_mutex_init(&_bug96_mutex1, NULL);
    pthread_cond_init(&_bug96_cond1, NULL);
    _bug96_ptr = NULL;
    _bug96_val = 0;
    _bug96_pred = 0;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug96_pred = 0;
        pthread_cond_signal(&_bug96_cond1);
        pthread_mutex_lock(&_bug96_mutex1);
        _bug96_ptr = &_bug96_val;
        _bug96_pred = 1;
        pthread_mutex_unlock(&_bug96_mutex1);
        pthread_cond_signal(&_bug96_cond1);
    }
    else {
        pthread_mutex_lock(&_bug96_mutex1);
        if (!_bug96_pred)
            pthread_cond_wait(&_bug96_cond1, &_bug96_mutex1);
        if (_bug96_ptr == NULL)
            trigger = 1;
        _bug96_ptr = NULL;
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
    static const uint32_t indices[] = {0x312};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x16d6edf3};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa120bece};
    static racebench_before_helper before_helpers[2] = {};
    _bug96_input_helper.interleave_num = 2;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xb7f7acc1;
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
static volatile __attribute__((used)) int* volatile _bug97_ptr;
static volatile __attribute__((used)) int _bug97_val;
static void _bug97_pattern_clean(void)
{
    pthread_mutex_init(&_bug97_mutex1, NULL);
    _bug97_ptr = NULL;
    _bug97_val = 0;
}
static __attribute__((optimize(0))) int _bug97_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug97_mutex1);
        _bug97_ptr = &_bug97_val;
        pthread_mutex_unlock(&_bug97_mutex1);
        pthread_mutex_lock(&_bug97_mutex1);
        if (_bug97_ptr == NULL)
            trigger = 1;
        _bug97_ptr = NULL;
        pthread_mutex_unlock(&_bug97_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug97_mutex1);
        _bug97_ptr = &_bug97_val;
        if (_bug97_ptr == NULL)
            trigger = 1;
        _bug97_ptr = NULL;
        pthread_mutex_unlock(&_bug97_mutex1);
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
    static const uint32_t indices[] = {0x2021};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x634ee559, 0x9d59954e};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11829ce};
    static racebench_before_helper before_helpers[3] = {};
    _bug97_input_helper.interleave_num = 3;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0xa1c0a475;
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
static pthread_mutex_t _bug98_mutex2;
static void _bug98_pattern_clean(void)
{
    pthread_mutex_init(&_bug98_mutex1, NULL);
    pthread_mutex_init(&_bug98_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug98_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug98_mutex1);
        if (pthread_mutex_trylock(&_bug98_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug98_mutex2);
        pthread_mutex_unlock(&_bug98_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug98_mutex2);
        if (pthread_mutex_trylock(&_bug98_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug98_mutex1);
        pthread_mutex_unlock(&_bug98_mutex2);
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
    static const uint32_t indices[] = {0x181c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug98_input_helper.interleave_num = 0;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0xa1182e1e;
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
static volatile __attribute__((used)) int _bug99_val;
static void _bug99_pattern_clean(void)
{
    pthread_mutex_init(&_bug99_mutex1, NULL);
    _bug99_val = 0;
}
static __attribute__((optimize(0))) int _bug99_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug99_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug99_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug99_mutex1);
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

static void _bug99_pattern_clean(void);
static int _bug99_pattern(int part);
static racebench_bug_helper _bug99_bug_helper;
static racebench_input_helper _bug99_input_helper;
__attribute__((constructor))
static void _bug99_input_helper_init(void)
{
    static const uint32_t indices[] = {0x927};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x3376cddf};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113fc1e};
    static racebench_before_helper before_helpers[2] = {};
    _bug99_input_helper.interleave_num = 2;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0xd48ac9fd;
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
