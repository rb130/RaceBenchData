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
static volatile __attribute__((used)) int _bug0_first_part;
static volatile __attribute__((used)) int _bug0_released;
static void _bug0_pattern_clean(void)
{
    pthread_mutex_init(&_bug0_mutex1, NULL);
    _bug0_first_part = -1;
    _bug0_released = -1;
}
static __attribute__((optimize(0))) int _bug0_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug0_mutex1) == 0)
                break;
            if (_bug0_released == 0) {
                _bug0_first_part = -1;
                _bug0_released = -1;
                return 1;
            }
        }
        if (_bug0_first_part == -1)
            _bug0_first_part = part;
        else {
            _bug0_first_part = -1;
            while (_bug0_released == -1) {}
            _bug0_released = -1;
        }
        pthread_mutex_unlock(&_bug0_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug0_mutex1);
        if (_bug0_first_part == -1) {
            _bug0_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug0_mutex1);
                _bug0_released = 1;
            }
            else {
                _bug0_released = 0;
                while (_bug0_released != -1) {}
                pthread_mutex_unlock(&_bug0_mutex1);
                return 1;
            }
        }
        else {
            _bug0_first_part = -1;
            pthread_mutex_unlock(&_bug0_mutex1);
        }
    }
    return 0;
}

static void _bug0_pattern_clean(void);
static int _bug0_pattern(int part);
static racebench_bug_helper _bug0_bug_helper;
static racebench_input_helper _bug0_input_helper;
__attribute__((constructor))
static void _bug0_input_helper_init(void)
{
    static const uint32_t indices[] = {0xb0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182a1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug0_input_helper.interleave_num = 1;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0xa1182a1e;
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
    static const uint32_t indices[] = {0x45};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[0] = {};
    _bug1_input_helper.interleave_num = 0;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xa1182cce;
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
static volatile __attribute__((used)) int _bug2_released;
static void _bug2_pattern_clean(void)
{
    pthread_mutex_init(&_bug2_mutex1, NULL);
    _bug2_first_part = -1;
    _bug2_released = -1;
}
static __attribute__((optimize(0))) int _bug2_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug2_mutex1) == 0)
                break;
            if (_bug2_released == 0) {
                _bug2_first_part = -1;
                _bug2_released = -1;
                return 1;
            }
        }
        if (_bug2_first_part == -1)
            _bug2_first_part = part;
        else {
            _bug2_first_part = -1;
            while (_bug2_released == -1) {}
            _bug2_released = -1;
        }
        pthread_mutex_unlock(&_bug2_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug2_mutex1);
        if (_bug2_first_part == -1) {
            _bug2_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug2_mutex1);
                _bug2_released = 1;
            }
            else {
                _bug2_released = 0;
                while (_bug2_released != -1) {}
                pthread_mutex_unlock(&_bug2_mutex1);
                return 1;
            }
        }
        else {
            _bug2_first_part = -1;
            pthread_mutex_unlock(&_bug2_mutex1);
        }
    }
    return 0;
}

static void _bug2_pattern_clean(void);
static int _bug2_pattern(int part);
static racebench_bug_helper _bug2_bug_helper;
static racebench_input_helper _bug2_input_helper;
__attribute__((constructor))
static void _bug2_input_helper_init(void)
{
    static const uint32_t indices[] = {0x30};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1145c9e};
    static racebench_before_helper before_helpers[1] = {};
    _bug2_input_helper.interleave_num = 1;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0xa1145c9e;
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
    static const uint32_t indices[] = {0x4b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x54973ba9};
    static racebench_before_helper before_helpers[1] = {};
    _bug3_input_helper.interleave_num = 1;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0x54973ba9;
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
    static const uint32_t indices[] = {0xda};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c5e};
    static racebench_before_helper before_helpers[1] = {};
    _bug4_input_helper.interleave_num = 1;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xa1182c5e;
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
    static const uint32_t indices[] = {0xd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa111f13e};
    static racebench_before_helper before_helpers[0] = {};
    _bug5_input_helper.interleave_num = 0;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xa111f13e;
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
static pthread_mutex_t _bug6_mutex2;
static void _bug6_pattern_clean(void)
{
    pthread_mutex_init(&_bug6_mutex1, NULL);
    pthread_mutex_init(&_bug6_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug6_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug6_mutex1);
        if (pthread_mutex_trylock(&_bug6_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug6_mutex2);
        pthread_mutex_unlock(&_bug6_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug6_mutex2);
        if (pthread_mutex_trylock(&_bug6_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug6_mutex1);
        pthread_mutex_unlock(&_bug6_mutex2);
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
    static const uint32_t indices[] = {0x99};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x66919ccc};
    static racebench_before_helper before_helpers[1] = {};
    _bug6_input_helper.interleave_num = 1;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0x66919ccc;
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
    static const uint32_t indices[] = {0x180};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2bc60656, 0x2de0dcd1};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115bc7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug7_input_helper.interleave_num = 3;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0xfabc9fa5;
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
static pthread_mutex_t _bug8_mutex2;
static void _bug8_pattern_clean(void)
{
    pthread_mutex_init(&_bug8_mutex1, NULL);
    pthread_mutex_init(&_bug8_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug8_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug8_mutex1);
        if (pthread_mutex_trylock(&_bug8_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug8_mutex2);
        pthread_mutex_unlock(&_bug8_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug8_mutex2);
        if (pthread_mutex_trylock(&_bug8_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug8_mutex1);
        pthread_mutex_unlock(&_bug8_mutex2);
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
    static const uint32_t indices[] = {0x4b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xf63347b4, 0x599ee279};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x3514e2f};
    static racebench_before_helper before_helpers[3] = {};
    _bug8_input_helper.interleave_num = 3;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0x5323785c;
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
    static const uint32_t indices[] = {0x59};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa116087e};
    static racebench_before_helper before_helpers[0] = {};
    _bug9_input_helper.interleave_num = 0;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0xa116087e;
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
static volatile __attribute__((used)) int _bug10_arr[3];
static volatile __attribute__((used)) int _bug10_val;
static volatile __attribute__((used)) int _bug10_pred;
static void _bug10_pattern_clean(void)
{
    _bug10_arr[0] = 0, _bug10_arr[1] = 2, _bug10_arr[2] = 1;
    _bug10_val = 0;
    _bug10_pred = 0;
}
static __attribute__((optimize(0))) int _bug10_pattern(int part)
{
    int trigger = 0;
    _bug10_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug10_val += _bug10_arr[i];
        }
        __sync_synchronize();
        while (!_bug10_pred)
            pthread_yield();
        trigger = _bug10_val != 3;
        _bug10_pred = 0;
        _bug10_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug10_val += _bug10_arr[i];
        }
        __sync_synchronize();
        _bug10_pred = 1;
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
    static const uint32_t indices[] = {0x1a1};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xd49479ea};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa119ec6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug10_input_helper.interleave_num = 2;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0x75ae6658;
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
    static const uint32_t indices[] = {0xad};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xace3a7ed, 0x40a87495};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xd1a47c24};
    static racebench_before_helper before_helpers[3] = {};
    _bug11_input_helper.interleave_num = 3;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0xbf3098a6;
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
    static const uint32_t indices[] = {0x70};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug12_input_helper.interleave_num = 0;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xa1182e4e;
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
static pthread_mutex_t _bug13_mutex2;
static void _bug13_pattern_clean(void)
{
    pthread_mutex_init(&_bug13_mutex1, NULL);
    pthread_mutex_init(&_bug13_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug13_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug13_mutex1);
        if (pthread_mutex_trylock(&_bug13_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug13_mutex2);
        pthread_mutex_unlock(&_bug13_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug13_mutex2);
        if (pthread_mutex_trylock(&_bug13_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug13_mutex1);
        pthread_mutex_unlock(&_bug13_mutex2);
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
    static const uint32_t indices[] = {0x78};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x356e3432, 0x54c4a996};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa112dfce};
    static racebench_before_helper before_helpers[3] = {};
    _bug13_input_helper.interleave_num = 3;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0x2b45bd96;
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
    static const uint32_t indices[] = {0x191};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1111abe};
    static racebench_before_helper before_helpers[1] = {};
    _bug14_input_helper.interleave_num = 1;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0xa1111abe;
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
static pthread_cond_t _bug15_cond1;
static volatile __attribute__((used)) int* volatile _bug15_ptr;
static volatile __attribute__((used)) int _bug15_val;
static volatile __attribute__((used)) int _bug15_pred;
static void _bug15_pattern_clean(void)
{
    pthread_mutex_init(&_bug15_mutex1, NULL);
    pthread_cond_init(&_bug15_cond1, NULL);
    _bug15_ptr = NULL;
    _bug15_val = 0;
    _bug15_pred = 0;
}
static __attribute__((optimize(0))) int _bug15_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug15_pred = 0;
        pthread_cond_signal(&_bug15_cond1);
        pthread_mutex_lock(&_bug15_mutex1);
        _bug15_ptr = &_bug15_val;
        _bug15_pred = 1;
        pthread_mutex_unlock(&_bug15_mutex1);
        pthread_cond_signal(&_bug15_cond1);
    }
    else {
        pthread_mutex_lock(&_bug15_mutex1);
        if (!_bug15_pred)
            pthread_cond_wait(&_bug15_cond1, &_bug15_mutex1);
        if (_bug15_ptr == NULL)
            trigger = 1;
        _bug15_ptr = NULL;
        _bug15_pred = 0;
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
    static const uint32_t indices[] = {0xa5};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1161c4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug15_input_helper.interleave_num = 0;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0xa1161c4e;
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
    static const uint32_t indices[] = {0xb0};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182bae};
    static racebench_before_helper before_helpers[0] = {};
    _bug16_input_helper.interleave_num = 0;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0xa1182bae;
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
static volatile __attribute__((used)) int _bug17_released;
static void _bug17_pattern_clean(void)
{
    pthread_mutex_init(&_bug17_mutex1, NULL);
    _bug17_first_part = -1;
    _bug17_released = -1;
}
static __attribute__((optimize(0))) int _bug17_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug17_mutex1) == 0)
                break;
            if (_bug17_released == 0) {
                _bug17_first_part = -1;
                _bug17_released = -1;
                return 1;
            }
        }
        if (_bug17_first_part == -1)
            _bug17_first_part = part;
        else {
            _bug17_first_part = -1;
            while (_bug17_released == -1) {}
            _bug17_released = -1;
        }
        pthread_mutex_unlock(&_bug17_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug17_mutex1);
        if (_bug17_first_part == -1) {
            _bug17_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug17_mutex1);
                _bug17_released = 1;
            }
            else {
                _bug17_released = 0;
                while (_bug17_released != -1) {}
                pthread_mutex_unlock(&_bug17_mutex1);
                return 1;
            }
        }
        else {
            _bug17_first_part = -1;
            pthread_mutex_unlock(&_bug17_mutex1);
        }
    }
    return 0;
}

static void _bug17_pattern_clean(void);
static int _bug17_pattern(int part);
static racebench_bug_helper _bug17_bug_helper;
static racebench_input_helper _bug17_input_helper;
__attribute__((constructor))
static void _bug17_input_helper_init(void)
{
    static const uint32_t indices[] = {0x111};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xbfdb3b7a};
    static racebench_before_helper before_helpers[0] = {};
    _bug17_input_helper.interleave_num = 0;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0xbfdb3b7a;
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
static volatile __attribute__((used)) int* volatile _bug18_ptr;
static volatile __attribute__((used)) int _bug18_val;
static void _bug18_pattern_clean(void)
{
    pthread_mutex_init(&_bug18_mutex1, NULL);
    _bug18_ptr = NULL;
    _bug18_val = 0;
}
static __attribute__((optimize(0))) int _bug18_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug18_mutex1);
        _bug18_ptr = &_bug18_val;
        pthread_mutex_unlock(&_bug18_mutex1);
        pthread_mutex_lock(&_bug18_mutex1);
        if (_bug18_ptr == NULL)
            trigger = 1;
        _bug18_ptr = NULL;
        pthread_mutex_unlock(&_bug18_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug18_mutex1);
        _bug18_ptr = &_bug18_val;
        if (_bug18_ptr == NULL)
            trigger = 1;
        _bug18_ptr = NULL;
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
    static const uint32_t indices[] = {0x76};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x608d15b7, 0x5f52f6b};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x5432acfc};
    static racebench_before_helper before_helpers[3] = {};
    _bug18_input_helper.interleave_num = 3;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0xbab4f21e;
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
static volatile __attribute__((used)) int _bug19_ok;
static volatile __attribute__((used)) int* volatile _bug19_ptr;
static volatile __attribute__((used)) int _bug19_val;
static void _bug19_pattern_clean(void)
{
    _bug19_ok = 0;
    _bug19_ptr = NULL;
    _bug19_val = 0;
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug19_ok = 1;
        __sync_synchronize();
        _bug19_ptr = &_bug19_val;
    }
    else {
        while (!_bug19_ok)
            pthread_yield();
        if (_bug19_ptr == NULL)
            trigger = 1;
        _bug19_ptr = NULL;
        _bug19_ok = 0;
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
    static const uint32_t indices[] = {0x4d};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x59685bbb, 0x6a9c5de2};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115982e};
    static racebench_before_helper before_helpers[3] = {};
    _bug19_input_helper.interleave_num = 3;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0x651a51cb;
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
static volatile __attribute__((used)) int _bug20_pred;
static volatile __attribute__((used)) int _bug20_sent;
static void _bug20_pattern_clean(void)
{
    pthread_mutex_init(&_bug20_mutex1, NULL);
    pthread_cond_init(&_bug20_cond1, NULL);
    _bug20_pred = 0;
    _bug20_sent = 0;
}
static __attribute__((optimize(0))) int _bug20_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug20_mutex1);
        _bug20_pred = 1;
        pthread_cond_signal(&_bug20_cond1);
        _bug20_sent = 1;
        pthread_mutex_unlock(&_bug20_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug20_mutex1);
        if (_bug20_sent) {
            trigger = 1;
        }
        else {
            while (_bug20_pred == 0)
                pthread_cond_wait(&_bug20_cond1, &_bug20_mutex1);
        }
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
    static const uint32_t indices[] = {0x129};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x879200f0};
    static racebench_before_helper before_helpers[0] = {};
    _bug20_input_helper.interleave_num = 0;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0x879200f0;
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
static volatile __attribute__((used)) int _bug21_arr[3];
static volatile __attribute__((used)) int _bug21_val;
static volatile __attribute__((used)) int _bug21_pred;
static void _bug21_pattern_clean(void)
{
    _bug21_arr[0] = 0, _bug21_arr[1] = 2, _bug21_arr[2] = 1;
    _bug21_val = 0;
    _bug21_pred = 0;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    int trigger = 0;
    _bug21_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug21_val += _bug21_arr[i];
        }
        __sync_synchronize();
        while (!_bug21_pred)
            pthread_yield();
        trigger = _bug21_val != 3;
        _bug21_pred = 0;
        _bug21_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug21_val += _bug21_arr[i];
        }
        __sync_synchronize();
        _bug21_pred = 1;
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
    static const uint32_t indices[] = {0x3b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x288341a6};
    static racebench_before_helper before_helpers[0] = {};
    _bug21_input_helper.interleave_num = 0;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0x288341a6;
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
    static const uint32_t indices[] = {0x114};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa113133e};
    static racebench_before_helper before_helpers[1] = {};
    _bug22_input_helper.interleave_num = 1;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0xa113133e;
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
    static const uint32_t indices[] = {0x11};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x9876480b};
    static racebench_before_helper before_helpers[0] = {};
    _bug23_input_helper.interleave_num = 0;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0x9876480b;
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
    static const uint32_t indices[] = {0x17e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1119f4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug24_input_helper.interleave_num = 0;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0xa1119f4e;
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
    static const uint32_t indices[] = {0x4d};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xfabb3130, 0xcafc008};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug25_input_helper.interleave_num = 3;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xa8832086;
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
static volatile __attribute__((used)) int _bug26_arr[3];
static volatile __attribute__((used)) int _bug26_val;
static volatile __attribute__((used)) int _bug26_pred;
static void _bug26_pattern_clean(void)
{
    _bug26_arr[0] = 0, _bug26_arr[1] = 2, _bug26_arr[2] = 1;
    _bug26_val = 0;
    _bug26_pred = 0;
}
static __attribute__((optimize(0))) int _bug26_pattern(int part)
{
    int trigger = 0;
    _bug26_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug26_val += _bug26_arr[i];
        }
        __sync_synchronize();
        while (!_bug26_pred)
            pthread_yield();
        trigger = _bug26_val != 3;
        _bug26_pred = 0;
        _bug26_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug26_val += _bug26_arr[i];
        }
        __sync_synchronize();
        _bug26_pred = 1;
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
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x5256c9f};
    static racebench_before_helper before_helpers[1] = {};
    _bug26_input_helper.interleave_num = 1;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0x5256c9f;
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
static pthread_mutex_t _bug27_mutex2;
static void _bug27_pattern_clean(void)
{
    pthread_mutex_init(&_bug27_mutex1, NULL);
    pthread_mutex_init(&_bug27_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug27_mutex1);
        if (pthread_mutex_trylock(&_bug27_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug27_mutex2);
        pthread_mutex_unlock(&_bug27_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug27_mutex2);
        if (pthread_mutex_trylock(&_bug27_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug27_mutex1);
        pthread_mutex_unlock(&_bug27_mutex2);
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
    static const uint32_t indices[] = {0x2e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xfb5e27f, 0x666d2811};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x6a2b77bf};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0xe04e824f;
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
    static const uint32_t indices[] = {0x10c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1164e5e};
    static racebench_before_helper before_helpers[1] = {};
    _bug28_input_helper.interleave_num = 1;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa1164e5e;
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
    static const uint32_t indices[] = {0x8c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x9d0c1c9e, 0xc56cb485};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xe60c0c8c};
    static racebench_before_helper before_helpers[3] = {};
    _bug29_input_helper.interleave_num = 3;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x4884ddaf;
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
    static const uint32_t indices[] = {0x27};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xaf06683a, 0x2a3771ea};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118312e};
    static racebench_before_helper before_helpers[3] = {};
    _bug30_input_helper.interleave_num = 3;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0x7a560b52;
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
static pthread_mutex_t _bug31_mutex2;
static void _bug31_pattern_clean(void)
{
    pthread_mutex_init(&_bug31_mutex1, NULL);
    pthread_mutex_init(&_bug31_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug31_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug31_mutex1);
        if (pthread_mutex_trylock(&_bug31_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug31_mutex2);
        pthread_mutex_unlock(&_bug31_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug31_mutex2);
        if (pthread_mutex_trylock(&_bug31_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug31_mutex1);
        pthread_mutex_unlock(&_bug31_mutex2);
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
    static const uint32_t indices[] = {0x17d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa86396f1};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x7047df2f};
    static racebench_before_helper before_helpers[2] = {};
    _bug31_input_helper.interleave_num = 2;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0x18ab7620;
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
    static const uint32_t indices[] = {0x28};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xbebb1a01};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11a6e2e};
    static racebench_before_helper before_helpers[2] = {};
    _bug32_input_helper.interleave_num = 2;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0x5fd5882f;
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
static void _bug33_pattern_clean(void)
{
    pthread_mutex_init(&_bug33_mutex1, NULL);
    _bug33_first_part = -1;
}
static __attribute__((optimize(0))) int _bug33_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug33_mutex1);
        if (_bug33_first_part == -1)
            _bug33_first_part = part;
        else
            _bug33_first_part = -1;
        pthread_mutex_unlock(&_bug33_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug33_mutex1);
        if (_bug33_first_part == -1) {
            _bug33_first_part = part;
            trigger = 1;
        }
        else
            _bug33_first_part = -1;
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
    static const uint32_t indices[] = {0x53};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x82d598bd};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11e8cbe};
    static racebench_before_helper before_helpers[2] = {};
    _bug33_input_helper.interleave_num = 2;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0x23f4257b;
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
    static const uint32_t indices[] = {0x19e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf1503fcd};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1154d1e};
    static racebench_before_helper before_helpers[2] = {};
    _bug34_input_helper.interleave_num = 2;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0x92658ceb;
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
    static const uint32_t indices[] = {0x19e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x7724c3cb, 0xfaf30656};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x66568c4b};
    static racebench_before_helper before_helpers[3] = {};
    _bug35_input_helper.interleave_num = 3;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xd86e566c;
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
static void _bug36_pattern_clean(void)
{
    pthread_mutex_init(&_bug36_mutex1, NULL);
    _bug36_first_part = -1;
}
static __attribute__((optimize(0))) int _bug36_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug36_mutex1);
        if (_bug36_first_part == -1)
            _bug36_first_part = part;
        else
            _bug36_first_part = -1;
        pthread_mutex_unlock(&_bug36_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug36_mutex1);
        if (_bug36_first_part == -1) {
            _bug36_first_part = part;
            trigger = 1;
        }
        else
            _bug36_first_part = -1;
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
    static const uint32_t indices[] = {0xca};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf16840c7};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1157c3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug36_input_helper.interleave_num = 2;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0x927dbd05;
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
    static const uint32_t indices[] = {0xfb};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug37_input_helper.interleave_num = 1;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0xa1182f1e;
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
    static const uint32_t indices[] = {0xdd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1189c2e};
    static racebench_before_helper before_helpers[0] = {};
    _bug38_input_helper.interleave_num = 0;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xa1189c2e;
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
    static const uint32_t indices[] = {0x16};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x35415eb9};
    static racebench_before_helper before_helpers[0] = {};
    _bug39_input_helper.interleave_num = 0;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0x35415eb9;
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
static volatile __attribute__((used)) int _bug40_ok;
static volatile __attribute__((used)) int* volatile _bug40_ptr;
static volatile __attribute__((used)) int _bug40_val;
static void _bug40_pattern_clean(void)
{
    _bug40_ok = 0;
    _bug40_ptr = NULL;
    _bug40_val = 0;
}
static __attribute__((optimize(0))) int _bug40_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug40_ok = 1;
        __sync_synchronize();
        _bug40_ptr = &_bug40_val;
    }
    else {
        while (!_bug40_ok)
            pthread_yield();
        if (_bug40_ptr == NULL)
            trigger = 1;
        _bug40_ptr = NULL;
        _bug40_ok = 0;
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
    static const uint32_t indices[] = {0x1e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1122c0e};
    static racebench_before_helper before_helpers[1] = {};
    _bug40_input_helper.interleave_num = 1;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xa1122c0e;
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
    static const uint32_t indices[] = {0x1e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11ba59e};
    static racebench_before_helper before_helpers[0] = {};
    _bug41_input_helper.interleave_num = 0;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa11ba59e;
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
static void _bug42_pattern_clean(void)
{
    pthread_mutex_init(&_bug42_mutex1, NULL);
    _bug42_first_part = -1;
}
static __attribute__((optimize(0))) int _bug42_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug42_mutex1);
        if (_bug42_first_part == -1)
            _bug42_first_part = part;
        else
            _bug42_first_part = -1;
        pthread_mutex_unlock(&_bug42_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug42_mutex1);
        if (_bug42_first_part == -1) {
            _bug42_first_part = part;
            trigger = 1;
        }
        else
            _bug42_first_part = -1;
        pthread_mutex_unlock(&_bug42_mutex1);
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
    static const uint32_t indices[] = {0x2c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x27d5d5bc};
    static racebench_before_helper before_helpers[1] = {};
    _bug42_input_helper.interleave_num = 1;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0x27d5d5bc;
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
static volatile __attribute__((used)) int _bug43_pred;
static volatile __attribute__((used)) int _bug43_sent;
static void _bug43_pattern_clean(void)
{
    pthread_mutex_init(&_bug43_mutex1, NULL);
    pthread_cond_init(&_bug43_cond1, NULL);
    _bug43_pred = 0;
    _bug43_sent = 0;
}
static __attribute__((optimize(0))) int _bug43_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug43_mutex1);
        _bug43_pred = 1;
        pthread_cond_signal(&_bug43_cond1);
        _bug43_sent = 1;
        pthread_mutex_unlock(&_bug43_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug43_mutex1);
        if (_bug43_sent) {
            trigger = 1;
        }
        else {
            while (_bug43_pred == 0)
                pthread_cond_wait(&_bug43_cond1, &_bug43_mutex1);
        }
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
    static const uint32_t indices[] = {0x14e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xea3f3260};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1157c6e};
    static racebench_before_helper before_helpers[2] = {};
    _bug43_input_helper.interleave_num = 2;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0x8b54aece;
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
static volatile __attribute__((used)) int _bug44_released;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    _bug44_first_part = -1;
    _bug44_released = -1;
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug44_mutex1) == 0)
                break;
            if (_bug44_released == 0) {
                _bug44_first_part = -1;
                _bug44_released = -1;
                return 1;
            }
        }
        if (_bug44_first_part == -1)
            _bug44_first_part = part;
        else {
            _bug44_first_part = -1;
            while (_bug44_released == -1) {}
            _bug44_released = -1;
        }
        pthread_mutex_unlock(&_bug44_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug44_mutex1);
        if (_bug44_first_part == -1) {
            _bug44_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug44_mutex1);
                _bug44_released = 1;
            }
            else {
                _bug44_released = 0;
                while (_bug44_released != -1) {}
                pthread_mutex_unlock(&_bug44_mutex1);
                return 1;
            }
        }
        else {
            _bug44_first_part = -1;
            pthread_mutex_unlock(&_bug44_mutex1);
        }
    }
    return 0;
}

static void _bug44_pattern_clean(void);
static int _bug44_pattern(int part);
static racebench_bug_helper _bug44_bug_helper;
static racebench_input_helper _bug44_input_helper;
__attribute__((constructor))
static void _bug44_input_helper_init(void)
{
    static const uint32_t indices[] = {0x114};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xbf5bc43};
    static racebench_before_helper before_helpers[1] = {};
    _bug44_input_helper.interleave_num = 1;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xbf5bc43;
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
static volatile __attribute__((used)) int _bug45_first_part;
static void _bug45_pattern_clean(void)
{
    pthread_mutex_init(&_bug45_mutex1, NULL);
    _bug45_first_part = -1;
}
static __attribute__((optimize(0))) int _bug45_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug45_mutex1);
        if (_bug45_first_part == -1)
            _bug45_first_part = part;
        else
            _bug45_first_part = -1;
        pthread_mutex_unlock(&_bug45_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug45_mutex1);
        if (_bug45_first_part == -1) {
            _bug45_first_part = part;
            trigger = 1;
        }
        else
            _bug45_first_part = -1;
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
    static const uint32_t indices[] = {0x83};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x43d17d4e};
    static racebench_before_helper before_helpers[1] = {};
    _bug45_input_helper.interleave_num = 1;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0x43d17d4e;
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
    static const uint32_t indices[] = {0x10e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xef8cc857, 0x9c3ce8bb};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa111fc7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug46_input_helper.interleave_num = 3;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0x2cdbad90;
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
    static const uint32_t indices[] = {0x48};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x4ccb0253};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e9e};
    static racebench_before_helper before_helpers[2] = {};
    _bug47_input_helper.interleave_num = 2;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0xede330f1;
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
static pthread_cond_t _bug48_cond1;
static volatile __attribute__((used)) int _bug48_pred;
static volatile __attribute__((used)) int _bug48_sent;
static void _bug48_pattern_clean(void)
{
    pthread_mutex_init(&_bug48_mutex1, NULL);
    pthread_cond_init(&_bug48_cond1, NULL);
    _bug48_pred = 0;
    _bug48_sent = 0;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug48_mutex1);
        _bug48_pred = 1;
        pthread_cond_signal(&_bug48_cond1);
        _bug48_sent = 1;
        pthread_mutex_unlock(&_bug48_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug48_mutex1);
        if (_bug48_sent) {
            trigger = 1;
        }
        else {
            while (_bug48_pred == 0)
                pthread_cond_wait(&_bug48_cond1, &_bug48_mutex1);
        }
        _bug48_pred = 0;
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
    static const uint32_t indices[] = {0x129};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x59596bea};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113ccae};
    static racebench_before_helper before_helpers[2] = {};
    _bug48_input_helper.interleave_num = 2;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0xfa6d3898;
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
    static const uint32_t indices[] = {0x172};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1155bce};
    static racebench_before_helper before_helpers[0] = {};
    _bug49_input_helper.interleave_num = 0;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0xa1155bce;
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
    static const uint32_t indices[] = {0x188};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x96957cef};
    static racebench_before_helper before_helpers[1] = {};
    _bug50_input_helper.interleave_num = 1;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0x96957cef;
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
    static const uint32_t indices[] = {0x1d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xac3f9aaf};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xf893ce29};
    static racebench_before_helper before_helpers[2] = {};
    _bug51_input_helper.interleave_num = 2;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0xa4d368d8;
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
    static const uint32_t indices[] = {0x188};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xebbeee14};
    static racebench_before_helper before_helpers[1] = {};
    _bug52_input_helper.interleave_num = 1;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0xebbeee14;
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
    static const uint32_t indices[] = {0x195};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x2f4348f6};
    static racebench_before_helper before_helpers[0] = {};
    _bug53_input_helper.interleave_num = 0;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0x2f4348f6;
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
static pthread_mutex_t _bug54_mutex2;
static void _bug54_pattern_clean(void)
{
    pthread_mutex_init(&_bug54_mutex1, NULL);
    pthread_mutex_init(&_bug54_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug54_mutex1);
        if (pthread_mutex_trylock(&_bug54_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug54_mutex2);
        pthread_mutex_unlock(&_bug54_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug54_mutex2);
        if (pthread_mutex_trylock(&_bug54_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug54_mutex1);
        pthread_mutex_unlock(&_bug54_mutex2);
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
    static const uint32_t indices[] = {0x18b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x347ec960, 0x934c4c74};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c5e};
    static racebench_before_helper before_helpers[3] = {};
    _bug54_input_helper.interleave_num = 3;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0x68e34232;
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
static pthread_mutex_t _bug55_mutex2;
static void _bug55_pattern_clean(void)
{
    pthread_mutex_init(&_bug55_mutex1, NULL);
    pthread_mutex_init(&_bug55_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug55_mutex1);
        if (pthread_mutex_trylock(&_bug55_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug55_mutex2);
        pthread_mutex_unlock(&_bug55_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug55_mutex2);
        if (pthread_mutex_trylock(&_bug55_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug55_mutex1);
        pthread_mutex_unlock(&_bug55_mutex2);
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
    static const uint32_t indices[] = {0x10e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8e075539, 0x1f93a01e};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d3e};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0x4eb32295;
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
static pthread_mutex_t _bug56_mutex2;
static void _bug56_pattern_clean(void)
{
    pthread_mutex_init(&_bug56_mutex1, NULL);
    pthread_mutex_init(&_bug56_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug56_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug56_mutex1);
        if (pthread_mutex_trylock(&_bug56_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug56_mutex2);
        pthread_mutex_unlock(&_bug56_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug56_mutex2);
        if (pthread_mutex_trylock(&_bug56_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug56_mutex1);
        pthread_mutex_unlock(&_bug56_mutex2);
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
    static const uint32_t indices[] = {0x13d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x64f353cf};
    static racebench_before_helper before_helpers[1] = {};
    _bug56_input_helper.interleave_num = 1;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0x64f353cf;
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
static volatile __attribute__((used)) int _bug57_arr[3];
static volatile __attribute__((used)) int _bug57_val;
static volatile __attribute__((used)) int _bug57_pred;
static void _bug57_pattern_clean(void)
{
    _bug57_arr[0] = 0, _bug57_arr[1] = 2, _bug57_arr[2] = 1;
    _bug57_val = 0;
    _bug57_pred = 0;
}
static __attribute__((optimize(0))) int _bug57_pattern(int part)
{
    int trigger = 0;
    _bug57_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug57_val += _bug57_arr[i];
        }
        __sync_synchronize();
        while (!_bug57_pred)
            pthread_yield();
        trigger = _bug57_val != 3;
        _bug57_pred = 0;
        _bug57_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug57_val += _bug57_arr[i];
        }
        __sync_synchronize();
        _bug57_pred = 1;
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
    static const uint32_t indices[] = {0x133};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x23cad0f4};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xb32aeb60};
    static racebench_before_helper before_helpers[2] = {};
    _bug57_input_helper.interleave_num = 2;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0xd6f5bc54;
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
static volatile __attribute__((used)) int* volatile _bug58_ptr;
static volatile __attribute__((used)) int _bug58_val;
static void _bug58_pattern_clean(void)
{
    pthread_mutex_init(&_bug58_mutex1, NULL);
    _bug58_ptr = NULL;
    _bug58_val = 0;
}
static __attribute__((optimize(0))) int _bug58_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug58_mutex1);
        _bug58_ptr = &_bug58_val;
        pthread_mutex_unlock(&_bug58_mutex1);
        pthread_mutex_lock(&_bug58_mutex1);
        if (_bug58_ptr == NULL)
            trigger = 1;
        _bug58_ptr = NULL;
        pthread_mutex_unlock(&_bug58_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug58_mutex1);
        _bug58_ptr = &_bug58_val;
        if (_bug58_ptr == NULL)
            trigger = 1;
        _bug58_ptr = NULL;
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
    static const uint32_t indices[] = {0x1b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xc7c1febc, 0xd1a5311b};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa116dc4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug58_input_helper.interleave_num = 3;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0x3a7e0c25;
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
static volatile __attribute__((used)) int _bug59_val;
static void _bug59_pattern_clean(void)
{
    pthread_mutex_init(&_bug59_mutex1, NULL);
    _bug59_val = 0;
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug59_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug59_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug59_mutex1);
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

static void _bug59_pattern_clean(void);
static int _bug59_pattern(int part);
static racebench_bug_helper _bug59_bug_helper;
static racebench_input_helper _bug59_input_helper;
__attribute__((constructor))
static void _bug59_input_helper_init(void)
{
    static const uint32_t indices[] = {0x1a2};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118311e};
    static racebench_before_helper before_helpers[0] = {};
    _bug59_input_helper.interleave_num = 0;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0xa118311e;
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
    static const uint32_t indices[] = {0x5c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x9bc5804b};
    static racebench_before_helper before_helpers[0] = {};
    _bug60_input_helper.interleave_num = 0;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0x9bc5804b;
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
    static const uint32_t indices[] = {0x116};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11827de};
    static racebench_before_helper before_helpers[1] = {};
    _bug61_input_helper.interleave_num = 1;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0xa11827de;
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
    static const uint32_t indices[] = {0x6b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x8139f80};
    static racebench_before_helper before_helpers[0] = {};
    _bug62_input_helper.interleave_num = 0;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0x8139f80;
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
static volatile __attribute__((used)) int _bug63_ok;
static volatile __attribute__((used)) int* volatile _bug63_ptr;
static volatile __attribute__((used)) int _bug63_val;
static void _bug63_pattern_clean(void)
{
    _bug63_ok = 0;
    _bug63_ptr = NULL;
    _bug63_val = 0;
}
static __attribute__((optimize(0))) int _bug63_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug63_ok = 1;
        __sync_synchronize();
        _bug63_ptr = &_bug63_val;
    }
    else {
        while (!_bug63_ok)
            pthread_yield();
        if (_bug63_ptr == NULL)
            trigger = 1;
        _bug63_ptr = NULL;
        _bug63_ok = 0;
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
    static const uint32_t indices[] = {0x185};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xb5157b2f};
    static racebench_before_helper before_helpers[1] = {};
    _bug63_input_helper.interleave_num = 1;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0xb5157b2f;
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
static volatile __attribute__((used)) int _bug64_released;
static void _bug64_pattern_clean(void)
{
    pthread_mutex_init(&_bug64_mutex1, NULL);
    _bug64_first_part = -1;
    _bug64_released = -1;
}
static __attribute__((optimize(0))) int _bug64_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug64_mutex1) == 0)
                break;
            if (_bug64_released == 0) {
                _bug64_first_part = -1;
                _bug64_released = -1;
                return 1;
            }
        }
        if (_bug64_first_part == -1)
            _bug64_first_part = part;
        else {
            _bug64_first_part = -1;
            while (_bug64_released == -1) {}
            _bug64_released = -1;
        }
        pthread_mutex_unlock(&_bug64_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug64_mutex1);
        if (_bug64_first_part == -1) {
            _bug64_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug64_mutex1);
                _bug64_released = 1;
            }
            else {
                _bug64_released = 0;
                while (_bug64_released != -1) {}
                pthread_mutex_unlock(&_bug64_mutex1);
                return 1;
            }
        }
        else {
            _bug64_first_part = -1;
            pthread_mutex_unlock(&_bug64_mutex1);
        }
    }
    return 0;
}

static void _bug64_pattern_clean(void);
static int _bug64_pattern(int part);
static racebench_bug_helper _bug64_bug_helper;
static racebench_input_helper _bug64_input_helper;
__attribute__((constructor))
static void _bug64_input_helper_init(void)
{
    static const uint32_t indices[] = {0x10};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb0491807};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118306e};
    static racebench_before_helper before_helpers[2] = {};
    _bug64_input_helper.interleave_num = 2;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0x51614875;
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
    static const uint32_t indices[] = {0x133};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x6c6226c2, 0xd9a2e3a7};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118afae};
    static racebench_before_helper before_helpers[3] = {};
    _bug65_input_helper.interleave_num = 3;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0xe71dba17;
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
static volatile __attribute__((used)) int _bug66_pred;
static volatile __attribute__((used)) int _bug66_sent;
static void _bug66_pattern_clean(void)
{
    pthread_mutex_init(&_bug66_mutex1, NULL);
    pthread_cond_init(&_bug66_cond1, NULL);
    _bug66_pred = 0;
    _bug66_sent = 0;
}
static __attribute__((optimize(0))) int _bug66_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug66_mutex1);
        _bug66_pred = 1;
        pthread_cond_signal(&_bug66_cond1);
        _bug66_sent = 1;
        pthread_mutex_unlock(&_bug66_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug66_mutex1);
        if (_bug66_sent) {
            trigger = 1;
        }
        else {
            while (_bug66_pred == 0)
                pthread_cond_wait(&_bug66_cond1, &_bug66_mutex1);
        }
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
    static const uint32_t indices[] = {0x196};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xb7152afd};
    static racebench_before_helper before_helpers[0] = {};
    _bug66_input_helper.interleave_num = 0;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0xb7152afd;
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
    static const uint32_t indices[] = {0x132};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xd890cb41};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c5e};
    static racebench_before_helper before_helpers[2] = {};
    _bug67_input_helper.interleave_num = 2;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0x79a8f79f;
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
    static const uint32_t indices[] = {0x17c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182dee};
    static racebench_before_helper before_helpers[0] = {};
    _bug68_input_helper.interleave_num = 0;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0xa1182dee;
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
static volatile __attribute__((used)) int _bug69_val;
static void _bug69_pattern_clean(void)
{
    pthread_mutex_init(&_bug69_mutex1, NULL);
    _bug69_val = 0;
}
static __attribute__((optimize(0))) int _bug69_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug69_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug69_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug69_mutex1);
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

static void _bug69_pattern_clean(void);
static int _bug69_pattern(int part);
static racebench_bug_helper _bug69_bug_helper;
static racebench_input_helper _bug69_input_helper;
__attribute__((constructor))
static void _bug69_input_helper_init(void)
{
    static const uint32_t indices[] = {0x64};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x55ba7b7d, 0x1e3e6472};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11821ee};
    static racebench_before_helper before_helpers[3] = {};
    _bug69_input_helper.interleave_num = 3;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0x151101dd;
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
static volatile __attribute__((used)) int* volatile _bug70_ptr;
static volatile __attribute__((used)) int _bug70_val;
static void _bug70_pattern_clean(void)
{
    pthread_mutex_init(&_bug70_mutex1, NULL);
    _bug70_ptr = NULL;
    _bug70_val = 0;
}
static __attribute__((optimize(0))) int _bug70_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug70_mutex1);
        _bug70_ptr = &_bug70_val;
        pthread_mutex_unlock(&_bug70_mutex1);
        pthread_mutex_lock(&_bug70_mutex1);
        if (_bug70_ptr == NULL)
            trigger = 1;
        _bug70_ptr = NULL;
        pthread_mutex_unlock(&_bug70_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug70_mutex1);
        _bug70_ptr = &_bug70_val;
        if (_bug70_ptr == NULL)
            trigger = 1;
        _bug70_ptr = NULL;
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
    static const uint32_t indices[] = {0xa2};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xc342900e};
    static racebench_before_helper before_helpers[0] = {};
    _bug70_input_helper.interleave_num = 0;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0xc342900e;
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
static volatile __attribute__((used)) int _bug71_ok;
static volatile __attribute__((used)) int* volatile _bug71_ptr;
static volatile __attribute__((used)) int _bug71_val;
static void _bug71_pattern_clean(void)
{
    _bug71_ok = 0;
    _bug71_ptr = NULL;
    _bug71_val = 0;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug71_ok = 1;
        __sync_synchronize();
        _bug71_ptr = &_bug71_val;
    }
    else {
        while (!_bug71_ok)
            pthread_yield();
        if (_bug71_ptr == NULL)
            trigger = 1;
        _bug71_ptr = NULL;
        _bug71_ok = 0;
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
    static const uint32_t indices[] = {0x13b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x3af784d2};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113056e};
    static racebench_before_helper before_helpers[2] = {};
    _bug71_input_helper.interleave_num = 2;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0xdc0a8a40;
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
static pthread_mutex_t _bug72_mutex2;
static void _bug72_pattern_clean(void)
{
    pthread_mutex_init(&_bug72_mutex1, NULL);
    pthread_mutex_init(&_bug72_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug72_mutex1);
        if (pthread_mutex_trylock(&_bug72_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug72_mutex2);
        pthread_mutex_unlock(&_bug72_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug72_mutex2);
        if (pthread_mutex_trylock(&_bug72_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug72_mutex1);
        pthread_mutex_unlock(&_bug72_mutex2);
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
    static const uint32_t indices[] = {0xcc};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c2e};
    static racebench_before_helper before_helpers[0] = {};
    _bug72_input_helper.interleave_num = 0;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xa1182c2e;
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
static volatile __attribute__((used)) int _bug73_arr[3];
static volatile __attribute__((used)) int _bug73_val;
static volatile __attribute__((used)) int _bug73_pred;
static void _bug73_pattern_clean(void)
{
    _bug73_arr[0] = 0, _bug73_arr[1] = 2, _bug73_arr[2] = 1;
    _bug73_val = 0;
    _bug73_pred = 0;
}
static __attribute__((optimize(0))) int _bug73_pattern(int part)
{
    int trigger = 0;
    _bug73_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug73_val += _bug73_arr[i];
        }
        __sync_synchronize();
        while (!_bug73_pred)
            pthread_yield();
        trigger = _bug73_val != 3;
        _bug73_pred = 0;
        _bug73_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug73_val += _bug73_arr[i];
        }
        __sync_synchronize();
        _bug73_pred = 1;
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
    static const uint32_t indices[] = {0x50};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa114913e};
    static racebench_before_helper before_helpers[0] = {};
    _bug73_input_helper.interleave_num = 0;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0xa114913e;
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
static volatile __attribute__((used)) int _bug74_val;
static void _bug74_pattern_clean(void)
{
    pthread_mutex_init(&_bug74_mutex1, NULL);
    _bug74_val = 0;
}
static __attribute__((optimize(0))) int _bug74_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug74_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug74_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug74_mutex1);
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

static void _bug74_pattern_clean(void);
static int _bug74_pattern(int part);
static racebench_bug_helper _bug74_bug_helper;
static racebench_input_helper _bug74_input_helper;
__attribute__((constructor))
static void _bug74_input_helper_init(void)
{
    static const uint32_t indices[] = {0xf};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x5443ecb9};
    static racebench_before_helper before_helpers[0] = {};
    _bug74_input_helper.interleave_num = 0;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0x5443ecb9;
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
    static const uint32_t indices[] = {0x70};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x978a4cbf};
    static racebench_before_helper before_helpers[0] = {};
    _bug75_input_helper.interleave_num = 0;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0x978a4cbf;
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
    static const uint32_t indices[] = {0x38};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa3eb279a};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x44757821};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0xe8609fbb;
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
static volatile __attribute__((used)) int _bug77_first_part;
static volatile __attribute__((used)) int _bug77_released;
static void _bug77_pattern_clean(void)
{
    pthread_mutex_init(&_bug77_mutex1, NULL);
    _bug77_first_part = -1;
    _bug77_released = -1;
}
static __attribute__((optimize(0))) int _bug77_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug77_mutex1) == 0)
                break;
            if (_bug77_released == 0) {
                _bug77_first_part = -1;
                _bug77_released = -1;
                return 1;
            }
        }
        if (_bug77_first_part == -1)
            _bug77_first_part = part;
        else {
            _bug77_first_part = -1;
            while (_bug77_released == -1) {}
            _bug77_released = -1;
        }
        pthread_mutex_unlock(&_bug77_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug77_mutex1);
        if (_bug77_first_part == -1) {
            _bug77_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug77_mutex1);
                _bug77_released = 1;
            }
            else {
                _bug77_released = 0;
                while (_bug77_released != -1) {}
                pthread_mutex_unlock(&_bug77_mutex1);
                return 1;
            }
        }
        else {
            _bug77_first_part = -1;
            pthread_mutex_unlock(&_bug77_mutex1);
        }
    }
    return 0;
}

static void _bug77_pattern_clean(void);
static int _bug77_pattern(int part);
static racebench_bug_helper _bug77_bug_helper;
static racebench_input_helper _bug77_input_helper;
__attribute__((constructor))
static void _bug77_input_helper_init(void)
{
    static const uint32_t indices[] = {0x11e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x789896d9};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1151b2e};
    static racebench_before_helper before_helpers[2] = {};
    _bug77_input_helper.interleave_num = 2;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0x19adb207;
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
static volatile __attribute__((used)) int _bug78_ok;
static volatile __attribute__((used)) int* volatile _bug78_ptr;
static volatile __attribute__((used)) int _bug78_val;
static void _bug78_pattern_clean(void)
{
    _bug78_ok = 0;
    _bug78_ptr = NULL;
    _bug78_val = 0;
}
static __attribute__((optimize(0))) int _bug78_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug78_ok = 1;
        __sync_synchronize();
        _bug78_ptr = &_bug78_val;
    }
    else {
        while (!_bug78_ok)
            pthread_yield();
        if (_bug78_ptr == NULL)
            trigger = 1;
        _bug78_ptr = NULL;
        _bug78_ok = 0;
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
    static const uint32_t indices[] = {0x157};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x1c3eaea0, 0x7e049700};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1163c9e};
    static racebench_before_helper before_helpers[3] = {};
    _bug78_input_helper.interleave_num = 3;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0x3b59823e;
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
static volatile __attribute__((used)) int _bug79_first_part;
static volatile __attribute__((used)) int _bug79_released;
static void _bug79_pattern_clean(void)
{
    pthread_mutex_init(&_bug79_mutex1, NULL);
    _bug79_first_part = -1;
    _bug79_released = -1;
}
static __attribute__((optimize(0))) int _bug79_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug79_mutex1) == 0)
                break;
            if (_bug79_released == 0) {
                _bug79_first_part = -1;
                _bug79_released = -1;
                return 1;
            }
        }
        if (_bug79_first_part == -1)
            _bug79_first_part = part;
        else {
            _bug79_first_part = -1;
            while (_bug79_released == -1) {}
            _bug79_released = -1;
        }
        pthread_mutex_unlock(&_bug79_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug79_mutex1);
        if (_bug79_first_part == -1) {
            _bug79_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug79_mutex1);
                _bug79_released = 1;
            }
            else {
                _bug79_released = 0;
                while (_bug79_released != -1) {}
                pthread_mutex_unlock(&_bug79_mutex1);
                return 1;
            }
        }
        else {
            _bug79_first_part = -1;
            pthread_mutex_unlock(&_bug79_mutex1);
        }
    }
    return 0;
}

static void _bug79_pattern_clean(void);
static int _bug79_pattern(int part);
static racebench_bug_helper _bug79_bug_helper;
static racebench_input_helper _bug79_input_helper;
__attribute__((constructor))
static void _bug79_input_helper_init(void)
{
    static const uint32_t indices[] = {0xa1};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x8518e5fc};
    static racebench_before_helper before_helpers[1] = {};
    _bug79_input_helper.interleave_num = 1;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0x8518e5fc;
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
static volatile __attribute__((used)) int* volatile _bug80_ptr;
static volatile __attribute__((used)) int _bug80_val;
static void _bug80_pattern_clean(void)
{
    pthread_mutex_init(&_bug80_mutex1, NULL);
    _bug80_ptr = NULL;
    _bug80_val = 0;
}
static __attribute__((optimize(0))) int _bug80_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug80_mutex1);
        _bug80_ptr = &_bug80_val;
        pthread_mutex_unlock(&_bug80_mutex1);
        pthread_mutex_lock(&_bug80_mutex1);
        if (_bug80_ptr == NULL)
            trigger = 1;
        _bug80_ptr = NULL;
        pthread_mutex_unlock(&_bug80_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug80_mutex1);
        _bug80_ptr = &_bug80_val;
        if (_bug80_ptr == NULL)
            trigger = 1;
        _bug80_ptr = NULL;
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
    static const uint32_t indices[] = {0x85};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x4a148325};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x8894c0ad};
    static racebench_before_helper before_helpers[2] = {};
    _bug80_input_helper.interleave_num = 2;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xd2a943d2;
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
    static const uint32_t indices[] = {0x17d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xee65f381, 0xc901d100};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa5161e3f};
    static racebench_before_helper before_helpers[3] = {};
    _bug81_input_helper.interleave_num = 3;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0x5c7de2c0;
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
static volatile __attribute__((used)) int _bug82_pred;
static volatile __attribute__((used)) int _bug82_sent;
static void _bug82_pattern_clean(void)
{
    pthread_mutex_init(&_bug82_mutex1, NULL);
    pthread_cond_init(&_bug82_cond1, NULL);
    _bug82_pred = 0;
    _bug82_sent = 0;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug82_mutex1);
        _bug82_pred = 1;
        pthread_cond_signal(&_bug82_cond1);
        _bug82_sent = 1;
        pthread_mutex_unlock(&_bug82_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug82_mutex1);
        if (_bug82_sent) {
            trigger = 1;
        }
        else {
            while (_bug82_pred == 0)
                pthread_cond_wait(&_bug82_cond1, &_bug82_mutex1);
        }
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
    static const uint32_t indices[] = {0xa1};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x309bc424, 0x3ae2479b};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x38463cbf};
    static racebench_before_helper before_helpers[3] = {};
    _bug82_input_helper.interleave_num = 3;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0xa3c4487e;
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
static pthread_cond_t _bug83_cond1;
static volatile __attribute__((used)) int* volatile _bug83_ptr;
static volatile __attribute__((used)) int _bug83_val;
static volatile __attribute__((used)) int _bug83_pred;
static void _bug83_pattern_clean(void)
{
    pthread_mutex_init(&_bug83_mutex1, NULL);
    pthread_cond_init(&_bug83_cond1, NULL);
    _bug83_ptr = NULL;
    _bug83_val = 0;
    _bug83_pred = 0;
}
static __attribute__((optimize(0))) int _bug83_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug83_pred = 0;
        pthread_cond_signal(&_bug83_cond1);
        pthread_mutex_lock(&_bug83_mutex1);
        _bug83_ptr = &_bug83_val;
        _bug83_pred = 1;
        pthread_mutex_unlock(&_bug83_mutex1);
        pthread_cond_signal(&_bug83_cond1);
    }
    else {
        pthread_mutex_lock(&_bug83_mutex1);
        if (!_bug83_pred)
            pthread_cond_wait(&_bug83_cond1, &_bug83_mutex1);
        if (_bug83_ptr == NULL)
            trigger = 1;
        _bug83_ptr = NULL;
        _bug83_pred = 0;
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
    static const uint32_t indices[] = {0x1a5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xe5b12d00};
    static racebench_before_helper before_helpers[1] = {};
    _bug83_input_helper.interleave_num = 1;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0xe5b12d00;
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
    static const uint32_t indices[] = {0x128};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xb397c933, 0x93d8c760};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e0e};
    static racebench_before_helper before_helpers[3] = {};
    _bug84_input_helper.interleave_num = 3;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xe888bea1;
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
static volatile __attribute__((used)) int* volatile _bug85_ptr;
static volatile __attribute__((used)) int _bug85_val;
static void _bug85_pattern_clean(void)
{
    pthread_mutex_init(&_bug85_mutex1, NULL);
    _bug85_ptr = NULL;
    _bug85_val = 0;
}
static __attribute__((optimize(0))) int _bug85_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug85_mutex1);
        _bug85_ptr = &_bug85_val;
        pthread_mutex_unlock(&_bug85_mutex1);
        pthread_mutex_lock(&_bug85_mutex1);
        if (_bug85_ptr == NULL)
            trigger = 1;
        _bug85_ptr = NULL;
        pthread_mutex_unlock(&_bug85_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug85_mutex1);
        _bug85_ptr = &_bug85_val;
        if (_bug85_ptr == NULL)
            trigger = 1;
        _bug85_ptr = NULL;
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
    static const uint32_t indices[] = {0x181};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x25351c09};
    static racebench_before_helper before_helpers[1] = {};
    _bug85_input_helper.interleave_num = 1;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0x25351c09;
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
static volatile __attribute__((used)) int _bug86_val;
static void _bug86_pattern_clean(void)
{
    pthread_mutex_init(&_bug86_mutex1, NULL);
    _bug86_val = 0;
}
static __attribute__((optimize(0))) int _bug86_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug86_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug86_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug86_mutex1);
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

static void _bug86_pattern_clean(void);
static int _bug86_pattern(int part);
static racebench_bug_helper _bug86_bug_helper;
static racebench_input_helper _bug86_input_helper;
__attribute__((constructor))
static void _bug86_input_helper_init(void)
{
    static const uint32_t indices[] = {0x22};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x82b2e5e0};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1131dce};
    static racebench_before_helper before_helpers[2] = {};
    _bug86_input_helper.interleave_num = 2;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x23c603ae;
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
static volatile __attribute__((used)) int _bug87_pred;
static volatile __attribute__((used)) int _bug87_sent;
static void _bug87_pattern_clean(void)
{
    pthread_mutex_init(&_bug87_mutex1, NULL);
    pthread_cond_init(&_bug87_cond1, NULL);
    _bug87_pred = 0;
    _bug87_sent = 0;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug87_mutex1);
        _bug87_pred = 1;
        pthread_cond_signal(&_bug87_cond1);
        _bug87_sent = 1;
        pthread_mutex_unlock(&_bug87_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug87_mutex1);
        if (_bug87_sent) {
            trigger = 1;
        }
        else {
            while (_bug87_pred == 0)
                pthread_cond_wait(&_bug87_cond1, &_bug87_mutex1);
        }
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
    static const uint32_t indices[] = {0x188};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x928c0fa4};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182b4e};
    static racebench_before_helper before_helpers[2] = {};
    _bug87_input_helper.interleave_num = 2;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0x33a43af2;
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
static volatile __attribute__((used)) int _bug88_val;
static void _bug88_pattern_clean(void)
{
    pthread_mutex_init(&_bug88_mutex1, NULL);
    _bug88_val = 0;
}
static __attribute__((optimize(0))) int _bug88_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug88_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug88_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug88_mutex1);
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

static void _bug88_pattern_clean(void);
static int _bug88_pattern(int part);
static racebench_bug_helper _bug88_bug_helper;
static racebench_input_helper _bug88_input_helper;
__attribute__((constructor))
static void _bug88_input_helper_init(void)
{
    static const uint32_t indices[] = {0x7a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa11b2cce};
    static racebench_before_helper before_helpers[1] = {};
    _bug88_input_helper.interleave_num = 1;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0xa11b2cce;
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
static volatile __attribute__((used)) int _bug89_first_part;
static void _bug89_pattern_clean(void)
{
    pthread_mutex_init(&_bug89_mutex1, NULL);
    _bug89_first_part = -1;
}
static __attribute__((optimize(0))) int _bug89_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug89_mutex1);
        if (_bug89_first_part == -1)
            _bug89_first_part = part;
        else
            _bug89_first_part = -1;
        pthread_mutex_unlock(&_bug89_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug89_mutex1);
        if (_bug89_first_part == -1) {
            _bug89_first_part = part;
            trigger = 1;
        }
        else
            _bug89_first_part = -1;
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
    static const uint32_t indices[] = {0x1a0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x3144de3b, 0xc42b2adc};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xd511905f};
    static racebench_before_helper before_helpers[3] = {};
    _bug89_input_helper.interleave_num = 3;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0xca819976;
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
static volatile __attribute__((used)) int _bug90_arr[3];
static volatile __attribute__((used)) int _bug90_val;
static volatile __attribute__((used)) int _bug90_pred;
static void _bug90_pattern_clean(void)
{
    _bug90_arr[0] = 0, _bug90_arr[1] = 2, _bug90_arr[2] = 1;
    _bug90_val = 0;
    _bug90_pred = 0;
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    _bug90_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug90_val += _bug90_arr[i];
        }
        __sync_synchronize();
        while (!_bug90_pred)
            pthread_yield();
        trigger = _bug90_val != 3;
        _bug90_pred = 0;
        _bug90_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug90_val += _bug90_arr[i];
        }
        __sync_synchronize();
        _bug90_pred = 1;
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
    static const uint32_t indices[] = {0x47};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0x2352fc4f};
    static racebench_before_helper before_helpers[1] = {};
    _bug90_input_helper.interleave_num = 1;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0x2352fc4f;
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
static volatile __attribute__((used)) int _bug91_val;
static void _bug91_pattern_clean(void)
{
    pthread_mutex_init(&_bug91_mutex1, NULL);
    _bug91_val = 0;
}
static __attribute__((optimize(0))) int _bug91_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug91_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug91_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug91_mutex1);
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

static void _bug91_pattern_clean(void);
static int _bug91_pattern(int part);
static racebench_bug_helper _bug91_bug_helper;
static racebench_input_helper _bug91_input_helper;
__attribute__((constructor))
static void _bug91_input_helper_init(void)
{
    static const uint32_t indices[] = {0x170};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x880e7e86, 0x26e8af1};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11829be};
    static racebench_before_helper before_helpers[3] = {};
    _bug91_input_helper.interleave_num = 3;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0x2b953335;
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
static volatile __attribute__((used)) int _bug92_first_part;
static void _bug92_pattern_clean(void)
{
    pthread_mutex_init(&_bug92_mutex1, NULL);
    _bug92_first_part = -1;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug92_mutex1);
        if (_bug92_first_part == -1)
            _bug92_first_part = part;
        else
            _bug92_first_part = -1;
        pthread_mutex_unlock(&_bug92_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug92_mutex1);
        if (_bug92_first_part == -1) {
            _bug92_first_part = part;
            trigger = 1;
        }
        else
            _bug92_first_part = -1;
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
    static const uint32_t indices[] = {0x6};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug92_input_helper.interleave_num = 1;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int _bug93_val;
static void _bug93_pattern_clean(void)
{
    pthread_mutex_init(&_bug93_mutex1, NULL);
    _bug93_val = 0;
}
static __attribute__((optimize(0))) int _bug93_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug93_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug93_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug93_mutex1);
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

static void _bug93_pattern_clean(void);
static int _bug93_pattern(int part);
static racebench_bug_helper _bug93_bug_helper;
static racebench_input_helper _bug93_input_helper;
__attribute__((constructor))
static void _bug93_input_helper_init(void)
{
    static const uint32_t indices[] = {0x117};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x8078d957, 0x29c8b733};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x354208c9};
    static racebench_before_helper before_helpers[3] = {};
    _bug93_input_helper.interleave_num = 3;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0xdf839953;
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
static volatile __attribute__((used)) int _bug94_ok;
static volatile __attribute__((used)) int* volatile _bug94_ptr;
static volatile __attribute__((used)) int _bug94_val;
static void _bug94_pattern_clean(void)
{
    _bug94_ok = 0;
    _bug94_ptr = NULL;
    _bug94_val = 0;
}
static __attribute__((optimize(0))) int _bug94_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug94_ok = 1;
        __sync_synchronize();
        _bug94_ptr = &_bug94_val;
    }
    else {
        while (!_bug94_ok)
            pthread_yield();
        if (_bug94_ptr == NULL)
            trigger = 1;
        _bug94_ptr = NULL;
        _bug94_ok = 0;
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
    static const uint32_t indices[] = {0x17e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x3bc9efb2, 0x54d7b7c8};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182d3e};
    static racebench_before_helper before_helpers[3] = {};
    _bug94_input_helper.interleave_num = 3;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0x31b9d4b8;
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
static pthread_cond_t _bug95_cond1;
static volatile __attribute__((used)) int _bug95_pred;
static volatile __attribute__((used)) int _bug95_sent;
static void _bug95_pattern_clean(void)
{
    pthread_mutex_init(&_bug95_mutex1, NULL);
    pthread_cond_init(&_bug95_cond1, NULL);
    _bug95_pred = 0;
    _bug95_sent = 0;
}
static __attribute__((optimize(0))) int _bug95_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug95_mutex1);
        _bug95_pred = 1;
        pthread_cond_signal(&_bug95_cond1);
        _bug95_sent = 1;
        pthread_mutex_unlock(&_bug95_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug95_mutex1);
        if (_bug95_sent) {
            trigger = 1;
        }
        else {
            while (_bug95_pred == 0)
                pthread_cond_wait(&_bug95_cond1, &_bug95_mutex1);
        }
        _bug95_pred = 0;
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
    static const uint32_t indices[] = {0x134};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182d1e};
    static racebench_before_helper before_helpers[1] = {};
    _bug95_input_helper.interleave_num = 1;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xa1182d1e;
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
    static const uint32_t indices[] = {0x156};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb6ee8e54, 0x643e234a};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0x366671cc};
    static racebench_before_helper before_helpers[3] = {};
    _bug96_input_helper.interleave_num = 3;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0x5193236a;
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
    static const uint32_t indices[] = {0x38};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf8d64c4f};
    static racebench_before_helper before_helpers[0] = {};
    _bug97_input_helper.interleave_num = 0;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0xf8d64c4f;
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
    static const uint32_t indices[] = {0x84};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa116402e};
    static racebench_before_helper before_helpers[1] = {};
    _bug98_input_helper.interleave_num = 1;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0xa116402e;
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
static volatile __attribute__((used)) int* volatile _bug99_ptr;
static volatile __attribute__((used)) int _bug99_val;
static void _bug99_pattern_clean(void)
{
    pthread_mutex_init(&_bug99_mutex1, NULL);
    _bug99_ptr = NULL;
    _bug99_val = 0;
}
static __attribute__((optimize(0))) int _bug99_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug99_mutex1);
        _bug99_ptr = &_bug99_val;
        pthread_mutex_unlock(&_bug99_mutex1);
        pthread_mutex_lock(&_bug99_mutex1);
        if (_bug99_ptr == NULL)
            trigger = 1;
        _bug99_ptr = NULL;
        pthread_mutex_unlock(&_bug99_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug99_mutex1);
        _bug99_ptr = &_bug99_val;
        if (_bug99_ptr == NULL)
            trigger = 1;
        _bug99_ptr = NULL;
        pthread_mutex_unlock(&_bug99_mutex1);
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
    static const uint32_t indices[] = {0x123};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x335b7c2c};
    static racebench_before_helper before_helpers[0] = {};
    _bug99_input_helper.interleave_num = 0;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0x335b7c2c;
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
