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
    static const uint32_t indices[] = {0x336c0};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x72e5ed6e};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11141be};
    static racebench_before_helper before_helpers[2] = {};
    _bug0_input_helper.interleave_num = 2;
    _bug0_input_helper.index_num = 1;
    _bug0_input_helper.indices = indices;
    _bug0_input_helper.sizes = sizes;
    _bug0_input_helper.pads = pads;
    _bug0_input_helper.before_parts = before_parts;
    _bug0_input_helper.mapped_inputs = mapped_inputs;
    _bug0_input_helper.before_helpers = before_helpers;
    _bug0_input_helper.magic_value = 0x13f72f2c;
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
static volatile __attribute__((used)) int _bug1_ok;
static volatile __attribute__((used)) int* volatile _bug1_ptr;
static volatile __attribute__((used)) int _bug1_val;
static void _bug1_pattern_clean(void)
{
    _bug1_ok = 0;
    _bug1_ptr = NULL;
    _bug1_val = 0;
}
static __attribute__((optimize(0))) int _bug1_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug1_ok = 1;
        __sync_synchronize();
        _bug1_ptr = &_bug1_val;
    }
    else {
        while (!_bug1_ok)
            pthread_yield();
        if (_bug1_ptr == NULL)
            trigger = 1;
        _bug1_ptr = NULL;
        _bug1_ok = 0;
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
    static const uint32_t indices[] = {0x255fc};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c5e};
    static racebench_before_helper before_helpers[0] = {};
    _bug1_input_helper.interleave_num = 0;
    _bug1_input_helper.index_num = 1;
    _bug1_input_helper.indices = indices;
    _bug1_input_helper.sizes = sizes;
    _bug1_input_helper.pads = pads;
    _bug1_input_helper.before_parts = before_parts;
    _bug1_input_helper.mapped_inputs = mapped_inputs;
    _bug1_input_helper.before_helpers = before_helpers;
    _bug1_input_helper.magic_value = 0xa1182c5e;
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
    static const uint32_t indices[] = {0x38be6};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x1d6d7f71};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c5e};
    static racebench_before_helper before_helpers[2] = {};
    _bug2_input_helper.interleave_num = 2;
    _bug2_input_helper.index_num = 1;
    _bug2_input_helper.indices = indices;
    _bug2_input_helper.sizes = sizes;
    _bug2_input_helper.pads = pads;
    _bug2_input_helper.before_parts = before_parts;
    _bug2_input_helper.mapped_inputs = mapped_inputs;
    _bug2_input_helper.before_helpers = before_helpers;
    _bug2_input_helper.magic_value = 0xbe85abcf;
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
    static const uint32_t indices[] = {0xaec1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[0] = {};
    _bug3_input_helper.interleave_num = 0;
    _bug3_input_helper.index_num = 1;
    _bug3_input_helper.indices = indices;
    _bug3_input_helper.sizes = sizes;
    _bug3_input_helper.pads = pads;
    _bug3_input_helper.before_parts = before_parts;
    _bug3_input_helper.mapped_inputs = mapped_inputs;
    _bug3_input_helper.before_helpers = before_helpers;
    _bug3_input_helper.magic_value = 0xa1182cce;
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
    static const uint32_t indices[] = {0x7c50};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4d82d3e6};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa112d37e};
    static racebench_before_helper before_helpers[2] = {};
    _bug4_input_helper.interleave_num = 2;
    _bug4_input_helper.index_num = 1;
    _bug4_input_helper.indices = indices;
    _bug4_input_helper.sizes = sizes;
    _bug4_input_helper.pads = pads;
    _bug4_input_helper.before_parts = before_parts;
    _bug4_input_helper.mapped_inputs = mapped_inputs;
    _bug4_input_helper.before_helpers = before_helpers;
    _bug4_input_helper.magic_value = 0xee95a764;
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
static volatile __attribute__((used)) int _bug5_val;
static void _bug5_pattern_clean(void)
{
    pthread_mutex_init(&_bug5_mutex1, NULL);
    _bug5_val = 0;
}
static __attribute__((optimize(0))) int _bug5_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug5_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug5_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug5_mutex1);
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

static void _bug5_pattern_clean(void);
static int _bug5_pattern(int part);
static racebench_bug_helper _bug5_bug_helper;
static racebench_input_helper _bug5_input_helper;
__attribute__((constructor))
static void _bug5_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2c092};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182efe};
    static racebench_before_helper before_helpers[1] = {};
    _bug5_input_helper.interleave_num = 1;
    _bug5_input_helper.index_num = 1;
    _bug5_input_helper.indices = indices;
    _bug5_input_helper.sizes = sizes;
    _bug5_input_helper.pads = pads;
    _bug5_input_helper.before_parts = before_parts;
    _bug5_input_helper.mapped_inputs = mapped_inputs;
    _bug5_input_helper.before_helpers = before_helpers;
    _bug5_input_helper.magic_value = 0xa1182efe;
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
    static const uint32_t indices[] = {0x374c9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xb99e8f2f};
    static racebench_before_helper before_helpers[1] = {};
    _bug6_input_helper.interleave_num = 1;
    _bug6_input_helper.index_num = 1;
    _bug6_input_helper.indices = indices;
    _bug6_input_helper.sizes = sizes;
    _bug6_input_helper.pads = pads;
    _bug6_input_helper.before_parts = before_parts;
    _bug6_input_helper.mapped_inputs = mapped_inputs;
    _bug6_input_helper.before_helpers = before_helpers;
    _bug6_input_helper.magic_value = 0xb99e8f2f;
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
    static const uint32_t indices[] = {0xddbd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdd2b0dab, 0x105258a5};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1178bfe};
    static racebench_before_helper before_helpers[3] = {};
    _bug7_input_helper.interleave_num = 3;
    _bug7_input_helper.index_num = 1;
    _bug7_input_helper.indices = indices;
    _bug7_input_helper.sizes = sizes;
    _bug7_input_helper.pads = pads;
    _bug7_input_helper.before_parts = before_parts;
    _bug7_input_helper.mapped_inputs = mapped_inputs;
    _bug7_input_helper.before_helpers = before_helpers;
    _bug7_input_helper.magic_value = 0x8e94f24e;
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
static volatile __attribute__((used)) int _bug8_released;
static void _bug8_pattern_clean(void)
{
    pthread_mutex_init(&_bug8_mutex1, NULL);
    _bug8_first_part = -1;
    _bug8_released = -1;
}
static __attribute__((optimize(0))) int _bug8_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug8_mutex1) == 0)
                break;
            if (_bug8_released == 0) {
                _bug8_first_part = -1;
                _bug8_released = -1;
                return 1;
            }
        }
        if (_bug8_first_part == -1)
            _bug8_first_part = part;
        else {
            _bug8_first_part = -1;
            while (_bug8_released == -1) {}
            _bug8_released = -1;
        }
        pthread_mutex_unlock(&_bug8_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug8_mutex1);
        if (_bug8_first_part == -1) {
            _bug8_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug8_mutex1);
                _bug8_released = 1;
            }
            else {
                _bug8_released = 0;
                while (_bug8_released != -1) {}
                pthread_mutex_unlock(&_bug8_mutex1);
                return 1;
            }
        }
        else {
            _bug8_first_part = -1;
            pthread_mutex_unlock(&_bug8_mutex1);
        }
    }
    return 0;
}

static void _bug8_pattern_clean(void);
static int _bug8_pattern(int part);
static racebench_bug_helper _bug8_bug_helper;
static racebench_input_helper _bug8_input_helper;
__attribute__((constructor))
static void _bug8_input_helper_init(void)
{
    static const uint32_t indices[] = {0x32d18};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xc7cd700f, 0x1c00d005};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1123f1e};
    static racebench_before_helper before_helpers[3] = {};
    _bug8_input_helper.interleave_num = 3;
    _bug8_input_helper.index_num = 1;
    _bug8_input_helper.indices = indices;
    _bug8_input_helper.sizes = sizes;
    _bug8_input_helper.pads = pads;
    _bug8_input_helper.before_parts = before_parts;
    _bug8_input_helper.mapped_inputs = mapped_inputs;
    _bug8_input_helper.before_helpers = before_helpers;
    _bug8_input_helper.magic_value = 0x84e07f32;
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
static volatile __attribute__((used)) int _bug9_ok;
static volatile __attribute__((used)) int* volatile _bug9_ptr;
static volatile __attribute__((used)) int _bug9_val;
static void _bug9_pattern_clean(void)
{
    _bug9_ok = 0;
    _bug9_ptr = NULL;
    _bug9_val = 0;
}
static __attribute__((optimize(0))) int _bug9_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug9_ok = 1;
        __sync_synchronize();
        _bug9_ptr = &_bug9_val;
    }
    else {
        while (!_bug9_ok)
            pthread_yield();
        if (_bug9_ptr == NULL)
            trigger = 1;
        _bug9_ptr = NULL;
        _bug9_ok = 0;
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
    static const uint32_t indices[] = {0x3196a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x6439a063};
    static racebench_before_helper before_helpers[0] = {};
    _bug9_input_helper.interleave_num = 0;
    _bug9_input_helper.index_num = 1;
    _bug9_input_helper.indices = indices;
    _bug9_input_helper.sizes = sizes;
    _bug9_input_helper.pads = pads;
    _bug9_input_helper.before_parts = before_parts;
    _bug9_input_helper.mapped_inputs = mapped_inputs;
    _bug9_input_helper.before_helpers = before_helpers;
    _bug9_input_helper.magic_value = 0x6439a063;
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
static volatile __attribute__((used)) int _bug10_pred;
static volatile __attribute__((used)) int _bug10_sent;
static void _bug10_pattern_clean(void)
{
    pthread_mutex_init(&_bug10_mutex1, NULL);
    pthread_cond_init(&_bug10_cond1, NULL);
    _bug10_pred = 0;
    _bug10_sent = 0;
}
static __attribute__((optimize(0))) int _bug10_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug10_mutex1);
        _bug10_pred = 1;
        pthread_cond_signal(&_bug10_cond1);
        _bug10_sent = 1;
        pthread_mutex_unlock(&_bug10_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug10_mutex1);
        if (_bug10_sent) {
            trigger = 1;
        }
        else {
            while (_bug10_pred == 0)
                pthread_cond_wait(&_bug10_cond1, &_bug10_mutex1);
        }
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
    static const uint32_t indices[] = {0x33daa};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x754d040f, 0xc3a5f777};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11eca3e};
    static racebench_before_helper before_helpers[3] = {};
    _bug10_input_helper.interleave_num = 3;
    _bug10_input_helper.index_num = 1;
    _bug10_input_helper.indices = indices;
    _bug10_input_helper.sizes = sizes;
    _bug10_input_helper.pads = pads;
    _bug10_input_helper.before_parts = before_parts;
    _bug10_input_helper.mapped_inputs = mapped_inputs;
    _bug10_input_helper.before_helpers = before_helpers;
    _bug10_input_helper.magic_value = 0xda11c5c4;
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
static volatile __attribute__((used)) int _bug11_arr[3];
static volatile __attribute__((used)) int _bug11_val;
static volatile __attribute__((used)) int _bug11_pred;
static void _bug11_pattern_clean(void)
{
    _bug11_arr[0] = 0, _bug11_arr[1] = 2, _bug11_arr[2] = 1;
    _bug11_val = 0;
    _bug11_pred = 0;
}
static __attribute__((optimize(0))) int _bug11_pattern(int part)
{
    int trigger = 0;
    _bug11_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug11_val += _bug11_arr[i];
        }
        __sync_synchronize();
        while (!_bug11_pred)
            pthread_yield();
        trigger = _bug11_val != 3;
        _bug11_pred = 0;
        _bug11_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug11_val += _bug11_arr[i];
        }
        __sync_synchronize();
        _bug11_pred = 1;
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
    static const uint32_t indices[] = {0x3ff5e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdf86a039};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa11d4cae};
    static racebench_before_helper before_helpers[2] = {};
    _bug11_input_helper.interleave_num = 2;
    _bug11_input_helper.index_num = 1;
    _bug11_input_helper.indices = indices;
    _bug11_input_helper.sizes = sizes;
    _bug11_input_helper.pads = pads;
    _bug11_input_helper.before_parts = before_parts;
    _bug11_input_helper.mapped_inputs = mapped_inputs;
    _bug11_input_helper.before_helpers = before_helpers;
    _bug11_input_helper.magic_value = 0x80a3ece7;
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
static volatile __attribute__((used)) int _bug12_arr[3];
static volatile __attribute__((used)) int _bug12_val;
static volatile __attribute__((used)) int _bug12_pred;
static void _bug12_pattern_clean(void)
{
    _bug12_arr[0] = 0, _bug12_arr[1] = 2, _bug12_arr[2] = 1;
    _bug12_val = 0;
    _bug12_pred = 0;
}
static __attribute__((optimize(0))) int _bug12_pattern(int part)
{
    int trigger = 0;
    _bug12_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug12_val += _bug12_arr[i];
        }
        __sync_synchronize();
        while (!_bug12_pred)
            pthread_yield();
        trigger = _bug12_val != 3;
        _bug12_pred = 0;
        _bug12_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug12_val += _bug12_arr[i];
        }
        __sync_synchronize();
        _bug12_pred = 1;
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
    static const uint32_t indices[] = {0x9149};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x4b870c47, 0x4f6b8f6};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11667ee};
    static racebench_before_helper before_helpers[3] = {};
    _bug12_input_helper.interleave_num = 3;
    _bug12_input_helper.index_num = 1;
    _bug12_input_helper.indices = indices;
    _bug12_input_helper.sizes = sizes;
    _bug12_input_helper.pads = pads;
    _bug12_input_helper.before_parts = before_parts;
    _bug12_input_helper.mapped_inputs = mapped_inputs;
    _bug12_input_helper.before_helpers = before_helpers;
    _bug12_input_helper.magic_value = 0xf1942d2b;
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
    static const uint32_t indices[] = {0x426fe};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xb218c8f8, 0x5373b05};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa120bb7e};
    static racebench_before_helper before_helpers[3] = {};
    _bug13_input_helper.interleave_num = 3;
    _bug13_input_helper.index_num = 1;
    _bug13_input_helper.indices = indices;
    _bug13_input_helper.sizes = sizes;
    _bug13_input_helper.pads = pads;
    _bug13_input_helper.before_parts = before_parts;
    _bug13_input_helper.mapped_inputs = mapped_inputs;
    _bug13_input_helper.before_helpers = before_helpers;
    _bug13_input_helper.magic_value = 0x5870bf7b;
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
    static const uint32_t indices[] = {0x34b3a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xa5cb72c5, 0x705ccfc3};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x34351c5a};
    static racebench_before_helper before_helpers[3] = {};
    _bug14_input_helper.interleave_num = 3;
    _bug14_input_helper.index_num = 1;
    _bug14_input_helper.indices = indices;
    _bug14_input_helper.sizes = sizes;
    _bug14_input_helper.pads = pads;
    _bug14_input_helper.before_parts = before_parts;
    _bug14_input_helper.mapped_inputs = mapped_inputs;
    _bug14_input_helper.before_helpers = before_helpers;
    _bug14_input_helper.magic_value = 0x4a5d5ee2;
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
    static const uint32_t indices[] = {0x328fa};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xddc1635};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1168ece};
    static racebench_before_helper before_helpers[2] = {};
    _bug15_input_helper.interleave_num = 2;
    _bug15_input_helper.index_num = 1;
    _bug15_input_helper.indices = indices;
    _bug15_input_helper.sizes = sizes;
    _bug15_input_helper.pads = pads;
    _bug15_input_helper.before_parts = before_parts;
    _bug15_input_helper.mapped_inputs = mapped_inputs;
    _bug15_input_helper.before_helpers = before_helpers;
    _bug15_input_helper.magic_value = 0xaef2a503;
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
    static const uint32_t indices[] = {0x10450};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xd7dd3e90};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug16_input_helper.interleave_num = 2;
    _bug16_input_helper.index_num = 1;
    _bug16_input_helper.indices = indices;
    _bug16_input_helper.sizes = sizes;
    _bug16_input_helper.pads = pads;
    _bug16_input_helper.before_parts = before_parts;
    _bug16_input_helper.mapped_inputs = mapped_inputs;
    _bug16_input_helper.before_helpers = before_helpers;
    _bug16_input_helper.magic_value = 0x78f56b0e;
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
    static const uint32_t indices[] = {0x2603c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x8f9dc3fe};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cce};
    static racebench_before_helper before_helpers[2] = {};
    _bug17_input_helper.interleave_num = 2;
    _bug17_input_helper.index_num = 1;
    _bug17_input_helper.indices = indices;
    _bug17_input_helper.sizes = sizes;
    _bug17_input_helper.pads = pads;
    _bug17_input_helper.before_parts = before_parts;
    _bug17_input_helper.mapped_inputs = mapped_inputs;
    _bug17_input_helper.before_helpers = before_helpers;
    _bug17_input_helper.magic_value = 0x30b5f0cc;
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
static volatile __attribute__((used)) int _bug18_arr[3];
static volatile __attribute__((used)) int _bug18_val;
static volatile __attribute__((used)) int _bug18_pred;
static void _bug18_pattern_clean(void)
{
    _bug18_arr[0] = 0, _bug18_arr[1] = 2, _bug18_arr[2] = 1;
    _bug18_val = 0;
    _bug18_pred = 0;
}
static __attribute__((optimize(0))) int _bug18_pattern(int part)
{
    int trigger = 0;
    _bug18_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug18_val += _bug18_arr[i];
        }
        __sync_synchronize();
        while (!_bug18_pred)
            pthread_yield();
        trigger = _bug18_val != 3;
        _bug18_pred = 0;
        _bug18_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug18_val += _bug18_arr[i];
        }
        __sync_synchronize();
        _bug18_pred = 1;
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
    static const uint32_t indices[] = {0x12f42};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xc4af9a79, 0x9b8763fa};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa115bcde};
    static racebench_before_helper before_helpers[3] = {};
    _bug18_input_helper.interleave_num = 3;
    _bug18_input_helper.index_num = 1;
    _bug18_input_helper.indices = indices;
    _bug18_input_helper.sizes = sizes;
    _bug18_input_helper.pads = pads;
    _bug18_input_helper.before_parts = before_parts;
    _bug18_input_helper.mapped_inputs = mapped_inputs;
    _bug18_input_helper.before_helpers = before_helpers;
    _bug18_input_helper.magic_value = 0x14cbb51;
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
static volatile __attribute__((used)) int _bug19_arr[3];
static volatile __attribute__((used)) int _bug19_val;
static volatile __attribute__((used)) int _bug19_pred;
static void _bug19_pattern_clean(void)
{
    _bug19_arr[0] = 0, _bug19_arr[1] = 2, _bug19_arr[2] = 1;
    _bug19_val = 0;
    _bug19_pred = 0;
}
static __attribute__((optimize(0))) int _bug19_pattern(int part)
{
    int trigger = 0;
    _bug19_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug19_val += _bug19_arr[i];
        }
        __sync_synchronize();
        while (!_bug19_pred)
            pthread_yield();
        trigger = _bug19_val != 3;
        _bug19_pred = 0;
        _bug19_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug19_val += _bug19_arr[i];
        }
        __sync_synchronize();
        _bug19_pred = 1;
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
    static const uint32_t indices[] = {0x28a04};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1170cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug19_input_helper.interleave_num = 1;
    _bug19_input_helper.index_num = 1;
    _bug19_input_helper.indices = indices;
    _bug19_input_helper.sizes = sizes;
    _bug19_input_helper.pads = pads;
    _bug19_input_helper.before_parts = before_parts;
    _bug19_input_helper.mapped_inputs = mapped_inputs;
    _bug19_input_helper.before_helpers = before_helpers;
    _bug19_input_helper.magic_value = 0xa1170cbe;
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
    static const uint32_t indices[] = {0x48009};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x99099a18, 0x683362f8};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11d61ce};
    static racebench_before_helper before_helpers[3] = {};
    _bug20_input_helper.interleave_num = 3;
    _bug20_input_helper.index_num = 1;
    _bug20_input_helper.indices = indices;
    _bug20_input_helper.sizes = sizes;
    _bug20_input_helper.pads = pads;
    _bug20_input_helper.before_parts = before_parts;
    _bug20_input_helper.mapped_inputs = mapped_inputs;
    _bug20_input_helper.before_helpers = before_helpers;
    _bug20_input_helper.magic_value = 0xa25a5ede;
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
static volatile __attribute__((used)) int _bug21_val;
static void _bug21_pattern_clean(void)
{
    pthread_mutex_init(&_bug21_mutex1, NULL);
    _bug21_val = 0;
}
static __attribute__((optimize(0))) int _bug21_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug21_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug21_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug21_mutex1);
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

static void _bug21_pattern_clean(void);
static int _bug21_pattern(int part);
static racebench_bug_helper _bug21_bug_helper;
static racebench_input_helper _bug21_input_helper;
__attribute__((constructor))
static void _bug21_input_helper_init(void)
{
    static const uint32_t indices[] = {0x40b0e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e7e};
    static racebench_before_helper before_helpers[1] = {};
    _bug21_input_helper.interleave_num = 1;
    _bug21_input_helper.index_num = 1;
    _bug21_input_helper.indices = indices;
    _bug21_input_helper.sizes = sizes;
    _bug21_input_helper.pads = pads;
    _bug21_input_helper.before_parts = before_parts;
    _bug21_input_helper.mapped_inputs = mapped_inputs;
    _bug21_input_helper.before_helpers = before_helpers;
    _bug21_input_helper.magic_value = 0xa1182e7e;
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
    static const uint32_t indices[] = {0x22a0e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x6493dcaa};
    static racebench_before_helper before_helpers[0] = {};
    _bug22_input_helper.interleave_num = 0;
    _bug22_input_helper.index_num = 1;
    _bug22_input_helper.indices = indices;
    _bug22_input_helper.sizes = sizes;
    _bug22_input_helper.pads = pads;
    _bug22_input_helper.before_parts = before_parts;
    _bug22_input_helper.mapped_inputs = mapped_inputs;
    _bug22_input_helper.before_helpers = before_helpers;
    _bug22_input_helper.magic_value = 0x6493dcaa;
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
static pthread_cond_t _bug23_cond1;
static volatile __attribute__((used)) int _bug23_pred;
static volatile __attribute__((used)) int _bug23_sent;
static void _bug23_pattern_clean(void)
{
    pthread_mutex_init(&_bug23_mutex1, NULL);
    pthread_cond_init(&_bug23_cond1, NULL);
    _bug23_pred = 0;
    _bug23_sent = 0;
}
static __attribute__((optimize(0))) int _bug23_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug23_mutex1);
        _bug23_pred = 1;
        pthread_cond_signal(&_bug23_cond1);
        _bug23_sent = 1;
        pthread_mutex_unlock(&_bug23_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug23_mutex1);
        if (_bug23_sent) {
            trigger = 1;
        }
        else {
            while (_bug23_pred == 0)
                pthread_cond_wait(&_bug23_cond1, &_bug23_mutex1);
        }
        _bug23_pred = 0;
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
    static const uint32_t indices[] = {0x36c2e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb3fca2ac};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x76c96b69};
    static racebench_before_helper before_helpers[2] = {};
    _bug23_input_helper.interleave_num = 2;
    _bug23_input_helper.index_num = 1;
    _bug23_input_helper.indices = indices;
    _bug23_input_helper.sizes = sizes;
    _bug23_input_helper.pads = pads;
    _bug23_input_helper.before_parts = before_parts;
    _bug23_input_helper.mapped_inputs = mapped_inputs;
    _bug23_input_helper.before_helpers = before_helpers;
    _bug23_input_helper.magic_value = 0x2ac60e15;
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
static volatile __attribute__((used)) int _bug24_released;
static void _bug24_pattern_clean(void)
{
    pthread_mutex_init(&_bug24_mutex1, NULL);
    _bug24_first_part = -1;
    _bug24_released = -1;
}
static __attribute__((optimize(0))) int _bug24_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug24_mutex1) == 0)
                break;
            if (_bug24_released == 0) {
                _bug24_first_part = -1;
                _bug24_released = -1;
                return 1;
            }
        }
        if (_bug24_first_part == -1)
            _bug24_first_part = part;
        else {
            _bug24_first_part = -1;
            while (_bug24_released == -1) {}
            _bug24_released = -1;
        }
        pthread_mutex_unlock(&_bug24_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug24_mutex1);
        if (_bug24_first_part == -1) {
            _bug24_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug24_mutex1);
                _bug24_released = 1;
            }
            else {
                _bug24_released = 0;
                while (_bug24_released != -1) {}
                pthread_mutex_unlock(&_bug24_mutex1);
                return 1;
            }
        }
        else {
            _bug24_first_part = -1;
            pthread_mutex_unlock(&_bug24_mutex1);
        }
    }
    return 0;
}

static void _bug24_pattern_clean(void);
static int _bug24_pattern(int part);
static racebench_bug_helper _bug24_bug_helper;
static racebench_input_helper _bug24_input_helper;
__attribute__((constructor))
static void _bug24_input_helper_init(void)
{
    static const uint32_t indices[] = {0x2874a};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x7f5d5c50, 0x2670a39e};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xe6c1fb4c};
    static racebench_before_helper before_helpers[3] = {};
    _bug24_input_helper.interleave_num = 3;
    _bug24_input_helper.index_num = 1;
    _bug24_input_helper.indices = indices;
    _bug24_input_helper.sizes = sizes;
    _bug24_input_helper.pads = pads;
    _bug24_input_helper.before_parts = before_parts;
    _bug24_input_helper.mapped_inputs = mapped_inputs;
    _bug24_input_helper.before_helpers = before_helpers;
    _bug24_input_helper.magic_value = 0x8c8ffb3a;
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
static pthread_cond_t _bug25_cond1;
static volatile __attribute__((used)) int _bug25_pred;
static volatile __attribute__((used)) int _bug25_sent;
static void _bug25_pattern_clean(void)
{
    pthread_mutex_init(&_bug25_mutex1, NULL);
    pthread_cond_init(&_bug25_cond1, NULL);
    _bug25_pred = 0;
    _bug25_sent = 0;
}
static __attribute__((optimize(0))) int _bug25_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug25_mutex1);
        _bug25_pred = 1;
        pthread_cond_signal(&_bug25_cond1);
        _bug25_sent = 1;
        pthread_mutex_unlock(&_bug25_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug25_mutex1);
        if (_bug25_sent) {
            trigger = 1;
        }
        else {
            while (_bug25_pred == 0)
                pthread_cond_wait(&_bug25_cond1, &_bug25_mutex1);
        }
        _bug25_pred = 0;
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
    static const uint32_t indices[] = {0x43c};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115a27e};
    static racebench_before_helper before_helpers[0] = {};
    _bug25_input_helper.interleave_num = 0;
    _bug25_input_helper.index_num = 1;
    _bug25_input_helper.indices = indices;
    _bug25_input_helper.sizes = sizes;
    _bug25_input_helper.pads = pads;
    _bug25_input_helper.before_parts = before_parts;
    _bug25_input_helper.mapped_inputs = mapped_inputs;
    _bug25_input_helper.before_helpers = before_helpers;
    _bug25_input_helper.magic_value = 0xa115a27e;
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
    static const uint32_t indices[] = {0x188e8};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1116f7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug26_input_helper.interleave_num = 0;
    _bug26_input_helper.index_num = 1;
    _bug26_input_helper.indices = indices;
    _bug26_input_helper.sizes = sizes;
    _bug26_input_helper.pads = pads;
    _bug26_input_helper.before_parts = before_parts;
    _bug26_input_helper.mapped_inputs = mapped_inputs;
    _bug26_input_helper.before_helpers = before_helpers;
    _bug26_input_helper.magic_value = 0xa1116f7e;
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
static volatile __attribute__((used)) int* volatile _bug27_ptr;
static volatile __attribute__((used)) int _bug27_val;
static void _bug27_pattern_clean(void)
{
    pthread_mutex_init(&_bug27_mutex1, NULL);
    _bug27_ptr = NULL;
    _bug27_val = 0;
}
static __attribute__((optimize(0))) int _bug27_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug27_mutex1);
        _bug27_ptr = &_bug27_val;
        pthread_mutex_unlock(&_bug27_mutex1);
        pthread_mutex_lock(&_bug27_mutex1);
        if (_bug27_ptr == NULL)
            trigger = 1;
        _bug27_ptr = NULL;
        pthread_mutex_unlock(&_bug27_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug27_mutex1);
        _bug27_ptr = &_bug27_val;
        if (_bug27_ptr == NULL)
            trigger = 1;
        _bug27_ptr = NULL;
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
    static const uint32_t indices[] = {0x3b8c7};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x7e5a9e96, 0x7026e395};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xc40afcd0};
    static racebench_before_helper before_helpers[3] = {};
    _bug27_input_helper.interleave_num = 3;
    _bug27_input_helper.index_num = 1;
    _bug27_input_helper.indices = indices;
    _bug27_input_helper.sizes = sizes;
    _bug27_input_helper.pads = pads;
    _bug27_input_helper.before_parts = before_parts;
    _bug27_input_helper.mapped_inputs = mapped_inputs;
    _bug27_input_helper.before_helpers = before_helpers;
    _bug27_input_helper.magic_value = 0xb28c7efb;
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
    static const uint32_t indices[] = {0x46c2f};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1161b4e};
    static racebench_before_helper before_helpers[0] = {};
    _bug28_input_helper.interleave_num = 0;
    _bug28_input_helper.index_num = 1;
    _bug28_input_helper.indices = indices;
    _bug28_input_helper.sizes = sizes;
    _bug28_input_helper.pads = pads;
    _bug28_input_helper.before_parts = before_parts;
    _bug28_input_helper.mapped_inputs = mapped_inputs;
    _bug28_input_helper.before_helpers = before_helpers;
    _bug28_input_helper.magic_value = 0xa1161b4e;
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
static volatile __attribute__((used)) int _bug29_ok;
static volatile __attribute__((used)) int* volatile _bug29_ptr;
static volatile __attribute__((used)) int _bug29_val;
static void _bug29_pattern_clean(void)
{
    _bug29_ok = 0;
    _bug29_ptr = NULL;
    _bug29_val = 0;
}
static __attribute__((optimize(0))) int _bug29_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug29_ok = 1;
        __sync_synchronize();
        _bug29_ptr = &_bug29_val;
    }
    else {
        while (!_bug29_ok)
            pthread_yield();
        if (_bug29_ptr == NULL)
            trigger = 1;
        _bug29_ptr = NULL;
        _bug29_ok = 0;
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
    static const uint32_t indices[] = {0x2ee6a};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xdd997117};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa11b307e};
    static racebench_before_helper before_helpers[2] = {};
    _bug29_input_helper.interleave_num = 2;
    _bug29_input_helper.index_num = 1;
    _bug29_input_helper.indices = indices;
    _bug29_input_helper.sizes = sizes;
    _bug29_input_helper.pads = pads;
    _bug29_input_helper.before_parts = before_parts;
    _bug29_input_helper.mapped_inputs = mapped_inputs;
    _bug29_input_helper.before_helpers = before_helpers;
    _bug29_input_helper.magic_value = 0x7eb4a195;
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
    static const uint32_t indices[] = {0x2a6f9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x36064f41};
    static racebench_before_helper before_helpers[0] = {};
    _bug30_input_helper.interleave_num = 0;
    _bug30_input_helper.index_num = 1;
    _bug30_input_helper.indices = indices;
    _bug30_input_helper.sizes = sizes;
    _bug30_input_helper.pads = pads;
    _bug30_input_helper.before_parts = before_parts;
    _bug30_input_helper.mapped_inputs = mapped_inputs;
    _bug30_input_helper.before_helpers = before_helpers;
    _bug30_input_helper.magic_value = 0x36064f41;
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
    static const uint32_t indices[] = {0x440a9};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x3348d3a};
    static racebench_before_helper before_helpers[0] = {};
    _bug31_input_helper.interleave_num = 0;
    _bug31_input_helper.index_num = 1;
    _bug31_input_helper.indices = indices;
    _bug31_input_helper.sizes = sizes;
    _bug31_input_helper.pads = pads;
    _bug31_input_helper.before_parts = before_parts;
    _bug31_input_helper.mapped_inputs = mapped_inputs;
    _bug31_input_helper.before_helpers = before_helpers;
    _bug31_input_helper.magic_value = 0x3348d3a;
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
static volatile __attribute__((used)) int _bug32_val;
static void _bug32_pattern_clean(void)
{
    pthread_mutex_init(&_bug32_mutex1, NULL);
    _bug32_val = 0;
}
static __attribute__((optimize(0))) int _bug32_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug32_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug32_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug32_mutex1);
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

static void _bug32_pattern_clean(void);
static int _bug32_pattern(int part);
static racebench_bug_helper _bug32_bug_helper;
static racebench_input_helper _bug32_input_helper;
__attribute__((constructor))
static void _bug32_input_helper_init(void)
{
    static const uint32_t indices[] = {0x106e0};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xb3691f9f};
    static racebench_before_helper before_helpers[1] = {};
    _bug32_input_helper.interleave_num = 1;
    _bug32_input_helper.index_num = 1;
    _bug32_input_helper.indices = indices;
    _bug32_input_helper.sizes = sizes;
    _bug32_input_helper.pads = pads;
    _bug32_input_helper.before_parts = before_parts;
    _bug32_input_helper.mapped_inputs = mapped_inputs;
    _bug32_input_helper.before_helpers = before_helpers;
    _bug32_input_helper.magic_value = 0xb3691f9f;
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
    static const uint32_t indices[] = {0x3f4d3};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf7033559, 0xefcfa7b3};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1148f4e};
    static racebench_before_helper before_helpers[3] = {};
    _bug33_input_helper.interleave_num = 3;
    _bug33_input_helper.index_num = 1;
    _bug33_input_helper.indices = indices;
    _bug33_input_helper.sizes = sizes;
    _bug33_input_helper.pads = pads;
    _bug33_input_helper.before_parts = before_parts;
    _bug33_input_helper.mapped_inputs = mapped_inputs;
    _bug33_input_helper.before_helpers = before_helpers;
    _bug33_input_helper.magic_value = 0x87e76c5a;
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
    static const uint32_t indices[] = {0x7a9c};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118230e};
    static racebench_before_helper before_helpers[0] = {};
    _bug34_input_helper.interleave_num = 0;
    _bug34_input_helper.index_num = 1;
    _bug34_input_helper.indices = indices;
    _bug34_input_helper.sizes = sizes;
    _bug34_input_helper.pads = pads;
    _bug34_input_helper.before_parts = before_parts;
    _bug34_input_helper.mapped_inputs = mapped_inputs;
    _bug34_input_helper.before_helpers = before_helpers;
    _bug34_input_helper.magic_value = 0xa118230e;
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
static volatile __attribute__((used)) int _bug35_ok;
static volatile __attribute__((used)) int* volatile _bug35_ptr;
static volatile __attribute__((used)) int _bug35_val;
static void _bug35_pattern_clean(void)
{
    _bug35_ok = 0;
    _bug35_ptr = NULL;
    _bug35_val = 0;
}
static __attribute__((optimize(0))) int _bug35_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug35_ok = 1;
        __sync_synchronize();
        _bug35_ptr = &_bug35_val;
    }
    else {
        while (!_bug35_ok)
            pthread_yield();
        if (_bug35_ptr == NULL)
            trigger = 1;
        _bug35_ptr = NULL;
        _bug35_ok = 0;
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
    static const uint32_t indices[] = {0x42b93};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1122d8e};
    static racebench_before_helper before_helpers[1] = {};
    _bug35_input_helper.interleave_num = 1;
    _bug35_input_helper.index_num = 1;
    _bug35_input_helper.indices = indices;
    _bug35_input_helper.sizes = sizes;
    _bug35_input_helper.pads = pads;
    _bug35_input_helper.before_parts = before_parts;
    _bug35_input_helper.mapped_inputs = mapped_inputs;
    _bug35_input_helper.before_helpers = before_helpers;
    _bug35_input_helper.magic_value = 0xa1122d8e;
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
    static const uint32_t indices[] = {0x15113};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xeebda14c};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa7b441b5};
    static racebench_before_helper before_helpers[2] = {};
    _bug36_input_helper.interleave_num = 2;
    _bug36_input_helper.index_num = 1;
    _bug36_input_helper.indices = indices;
    _bug36_input_helper.sizes = sizes;
    _bug36_input_helper.pads = pads;
    _bug36_input_helper.before_parts = before_parts;
    _bug36_input_helper.mapped_inputs = mapped_inputs;
    _bug36_input_helper.before_helpers = before_helpers;
    _bug36_input_helper.magic_value = 0x9671e301;
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
static pthread_mutex_t _bug37_mutex2;
static void _bug37_pattern_clean(void)
{
    pthread_mutex_init(&_bug37_mutex1, NULL);
    pthread_mutex_init(&_bug37_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug37_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug37_mutex1);
        if (pthread_mutex_trylock(&_bug37_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug37_mutex2);
        pthread_mutex_unlock(&_bug37_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug37_mutex2);
        if (pthread_mutex_trylock(&_bug37_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug37_mutex1);
        pthread_mutex_unlock(&_bug37_mutex2);
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
    static const uint32_t indices[] = {0x33138};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xaca079b1, 0x4cfc405e};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182fbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug37_input_helper.interleave_num = 3;
    _bug37_input_helper.index_num = 1;
    _bug37_input_helper.indices = indices;
    _bug37_input_helper.sizes = sizes;
    _bug37_input_helper.pads = pads;
    _bug37_input_helper.before_parts = before_parts;
    _bug37_input_helper.mapped_inputs = mapped_inputs;
    _bug37_input_helper.before_helpers = before_helpers;
    _bug37_input_helper.magic_value = 0x9ab4e9cd;
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
    static const uint32_t indices[] = {0x3d06d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xf433ed6c};
    static racebench_before_helper before_helpers[0] = {};
    _bug38_input_helper.interleave_num = 0;
    _bug38_input_helper.index_num = 1;
    _bug38_input_helper.indices = indices;
    _bug38_input_helper.sizes = sizes;
    _bug38_input_helper.pads = pads;
    _bug38_input_helper.before_parts = before_parts;
    _bug38_input_helper.mapped_inputs = mapped_inputs;
    _bug38_input_helper.before_helpers = before_helpers;
    _bug38_input_helper.magic_value = 0xf433ed6c;
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
static volatile __attribute__((used)) int _bug39_first_part;
static void _bug39_pattern_clean(void)
{
    pthread_mutex_init(&_bug39_mutex1, NULL);
    _bug39_first_part = -1;
}
static __attribute__((optimize(0))) int _bug39_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug39_mutex1);
        if (_bug39_first_part == -1)
            _bug39_first_part = part;
        else
            _bug39_first_part = -1;
        pthread_mutex_unlock(&_bug39_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug39_mutex1);
        if (_bug39_first_part == -1) {
            _bug39_first_part = part;
            trigger = 1;
        }
        else
            _bug39_first_part = -1;
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
    static const uint32_t indices[] = {0x26bd};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xd91be89d};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa119e8de};
    static racebench_before_helper before_helpers[2] = {};
    _bug39_input_helper.interleave_num = 2;
    _bug39_input_helper.index_num = 1;
    _bug39_input_helper.indices = indices;
    _bug39_input_helper.sizes = sizes;
    _bug39_input_helper.pads = pads;
    _bug39_input_helper.before_parts = before_parts;
    _bug39_input_helper.mapped_inputs = mapped_inputs;
    _bug39_input_helper.before_helpers = before_helpers;
    _bug39_input_helper.magic_value = 0x7a35d17b;
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
    static const uint32_t indices[] = {0x3c16a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118255e};
    static racebench_before_helper before_helpers[0] = {};
    _bug40_input_helper.interleave_num = 0;
    _bug40_input_helper.index_num = 1;
    _bug40_input_helper.indices = indices;
    _bug40_input_helper.sizes = sizes;
    _bug40_input_helper.pads = pads;
    _bug40_input_helper.before_parts = before_parts;
    _bug40_input_helper.mapped_inputs = mapped_inputs;
    _bug40_input_helper.before_helpers = before_helpers;
    _bug40_input_helper.magic_value = 0xa118255e;
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
    static const uint32_t indices[] = {0x3747b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cde};
    static racebench_before_helper before_helpers[1] = {};
    _bug41_input_helper.interleave_num = 1;
    _bug41_input_helper.index_num = 1;
    _bug41_input_helper.indices = indices;
    _bug41_input_helper.sizes = sizes;
    _bug41_input_helper.pads = pads;
    _bug41_input_helper.before_parts = before_parts;
    _bug41_input_helper.mapped_inputs = mapped_inputs;
    _bug41_input_helper.before_helpers = before_helpers;
    _bug41_input_helper.magic_value = 0xa1182cde;
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
    static const uint32_t indices[] = {0x32be4};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x38e23fec};
    static racebench_before_helper before_helpers[0] = {};
    _bug42_input_helper.interleave_num = 0;
    _bug42_input_helper.index_num = 1;
    _bug42_input_helper.indices = indices;
    _bug42_input_helper.sizes = sizes;
    _bug42_input_helper.pads = pads;
    _bug42_input_helper.before_parts = before_parts;
    _bug42_input_helper.mapped_inputs = mapped_inputs;
    _bug42_input_helper.before_helpers = before_helpers;
    _bug42_input_helper.magic_value = 0x38e23fec;
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
    static const uint32_t indices[] = {0xa775};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x80fb9787, 0x66f8d90e};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[3] = {};
    _bug43_input_helper.interleave_num = 3;
    _bug43_input_helper.index_num = 1;
    _bug43_input_helper.indices = indices;
    _bug43_input_helper.sizes = sizes;
    _bug43_input_helper.pads = pads;
    _bug43_input_helper.before_parts = before_parts;
    _bug43_input_helper.mapped_inputs = mapped_inputs;
    _bug43_input_helper.before_helpers = before_helpers;
    _bug43_input_helper.magic_value = 0x890c9d53;
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
static pthread_mutex_t _bug44_mutex2;
static void _bug44_pattern_clean(void)
{
    pthread_mutex_init(&_bug44_mutex1, NULL);
    pthread_mutex_init(&_bug44_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug44_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug44_mutex1);
        if (pthread_mutex_trylock(&_bug44_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug44_mutex2);
        pthread_mutex_unlock(&_bug44_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug44_mutex2);
        if (pthread_mutex_trylock(&_bug44_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug44_mutex1);
        pthread_mutex_unlock(&_bug44_mutex2);
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
    static const uint32_t indices[] = {0xedc9};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11f2c6e};
    static racebench_before_helper before_helpers[0] = {};
    _bug44_input_helper.interleave_num = 0;
    _bug44_input_helper.index_num = 1;
    _bug44_input_helper.indices = indices;
    _bug44_input_helper.sizes = sizes;
    _bug44_input_helper.pads = pads;
    _bug44_input_helper.before_parts = before_parts;
    _bug44_input_helper.mapped_inputs = mapped_inputs;
    _bug44_input_helper.before_helpers = before_helpers;
    _bug44_input_helper.magic_value = 0xa11f2c6e;
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
    static const uint32_t indices[] = {0xed01};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xe325a87f};
    static racebench_before_helper before_helpers[1] = {};
    _bug45_input_helper.interleave_num = 1;
    _bug45_input_helper.index_num = 1;
    _bug45_input_helper.indices = indices;
    _bug45_input_helper.sizes = sizes;
    _bug45_input_helper.pads = pads;
    _bug45_input_helper.before_parts = before_parts;
    _bug45_input_helper.mapped_inputs = mapped_inputs;
    _bug45_input_helper.before_helpers = before_helpers;
    _bug45_input_helper.magic_value = 0xe325a87f;
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
static pthread_cond_t _bug46_cond1;
static volatile __attribute__((used)) int* volatile _bug46_ptr;
static volatile __attribute__((used)) int _bug46_val;
static volatile __attribute__((used)) int _bug46_pred;
static void _bug46_pattern_clean(void)
{
    pthread_mutex_init(&_bug46_mutex1, NULL);
    pthread_cond_init(&_bug46_cond1, NULL);
    _bug46_ptr = NULL;
    _bug46_val = 0;
    _bug46_pred = 0;
}
static __attribute__((optimize(0))) int _bug46_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug46_pred = 0;
        pthread_cond_signal(&_bug46_cond1);
        pthread_mutex_lock(&_bug46_mutex1);
        _bug46_ptr = &_bug46_val;
        _bug46_pred = 1;
        pthread_mutex_unlock(&_bug46_mutex1);
        pthread_cond_signal(&_bug46_cond1);
    }
    else {
        pthread_mutex_lock(&_bug46_mutex1);
        if (!_bug46_pred)
            pthread_cond_wait(&_bug46_cond1, &_bug46_mutex1);
        if (_bug46_ptr == NULL)
            trigger = 1;
        _bug46_ptr = NULL;
        _bug46_pred = 0;
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
    static const uint32_t indices[] = {0x3be1};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa118307e};
    static racebench_before_helper before_helpers[1] = {};
    _bug46_input_helper.interleave_num = 1;
    _bug46_input_helper.index_num = 1;
    _bug46_input_helper.indices = indices;
    _bug46_input_helper.sizes = sizes;
    _bug46_input_helper.pads = pads;
    _bug46_input_helper.before_parts = before_parts;
    _bug46_input_helper.mapped_inputs = mapped_inputs;
    _bug46_input_helper.before_helpers = before_helpers;
    _bug46_input_helper.magic_value = 0xa118307e;
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
    static const uint32_t indices[] = {0x1bf56};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x45b17bbc};
    static racebench_before_helper before_helpers[0] = {};
    _bug47_input_helper.interleave_num = 0;
    _bug47_input_helper.index_num = 1;
    _bug47_input_helper.indices = indices;
    _bug47_input_helper.sizes = sizes;
    _bug47_input_helper.pads = pads;
    _bug47_input_helper.before_parts = before_parts;
    _bug47_input_helper.mapped_inputs = mapped_inputs;
    _bug47_input_helper.before_helpers = before_helpers;
    _bug47_input_helper.magic_value = 0x45b17bbc;
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
static volatile __attribute__((used)) int* volatile _bug48_ptr;
static volatile __attribute__((used)) int _bug48_val;
static void _bug48_pattern_clean(void)
{
    pthread_mutex_init(&_bug48_mutex1, NULL);
    _bug48_ptr = NULL;
    _bug48_val = 0;
}
static __attribute__((optimize(0))) int _bug48_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug48_mutex1);
        _bug48_ptr = &_bug48_val;
        pthread_mutex_unlock(&_bug48_mutex1);
        pthread_mutex_lock(&_bug48_mutex1);
        if (_bug48_ptr == NULL)
            trigger = 1;
        _bug48_ptr = NULL;
        pthread_mutex_unlock(&_bug48_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug48_mutex1);
        _bug48_ptr = &_bug48_val;
        if (_bug48_ptr == NULL)
            trigger = 1;
        _bug48_ptr = NULL;
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
    static const uint32_t indices[] = {0x2342b};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xc07a13f1};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x747690e3};
    static racebench_before_helper before_helpers[2] = {};
    _bug48_input_helper.interleave_num = 2;
    _bug48_input_helper.index_num = 1;
    _bug48_input_helper.indices = indices;
    _bug48_input_helper.sizes = sizes;
    _bug48_input_helper.pads = pads;
    _bug48_input_helper.before_parts = before_parts;
    _bug48_input_helper.mapped_inputs = mapped_inputs;
    _bug48_input_helper.before_helpers = before_helpers;
    _bug48_input_helper.magic_value = 0x34f0a4d4;
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
    static const uint32_t indices[] = {0xeaba};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x716f3741, 0x9a69618f};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x37667c50};
    static racebench_before_helper before_helpers[3] = {};
    _bug49_input_helper.interleave_num = 3;
    _bug49_input_helper.index_num = 1;
    _bug49_input_helper.indices = indices;
    _bug49_input_helper.sizes = sizes;
    _bug49_input_helper.pads = pads;
    _bug49_input_helper.before_parts = before_parts;
    _bug49_input_helper.mapped_inputs = mapped_inputs;
    _bug49_input_helper.before_helpers = before_helpers;
    _bug49_input_helper.magic_value = 0x433f1520;
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
static volatile __attribute__((used)) int* volatile _bug50_ptr;
static volatile __attribute__((used)) int _bug50_val;
static void _bug50_pattern_clean(void)
{
    pthread_mutex_init(&_bug50_mutex1, NULL);
    _bug50_ptr = NULL;
    _bug50_val = 0;
}
static __attribute__((optimize(0))) int _bug50_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug50_mutex1);
        _bug50_ptr = &_bug50_val;
        pthread_mutex_unlock(&_bug50_mutex1);
        pthread_mutex_lock(&_bug50_mutex1);
        if (_bug50_ptr == NULL)
            trigger = 1;
        _bug50_ptr = NULL;
        pthread_mutex_unlock(&_bug50_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug50_mutex1);
        _bug50_ptr = &_bug50_val;
        if (_bug50_ptr == NULL)
            trigger = 1;
        _bug50_ptr = NULL;
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
    static const uint32_t indices[] = {0x165ed};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x33f858df};
    static racebench_before_helper before_helpers[1] = {};
    _bug50_input_helper.interleave_num = 1;
    _bug50_input_helper.index_num = 1;
    _bug50_input_helper.indices = indices;
    _bug50_input_helper.sizes = sizes;
    _bug50_input_helper.pads = pads;
    _bug50_input_helper.before_parts = before_parts;
    _bug50_input_helper.mapped_inputs = mapped_inputs;
    _bug50_input_helper.before_helpers = before_helpers;
    _bug50_input_helper.magic_value = 0x33f858df;
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
    static const uint32_t indices[] = {0x277fb};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xdff165d2, 0x23e03c44};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x52554390};
    static racebench_before_helper before_helpers[3] = {};
    _bug51_input_helper.interleave_num = 3;
    _bug51_input_helper.index_num = 1;
    _bug51_input_helper.indices = indices;
    _bug51_input_helper.sizes = sizes;
    _bug51_input_helper.pads = pads;
    _bug51_input_helper.before_parts = before_parts;
    _bug51_input_helper.mapped_inputs = mapped_inputs;
    _bug51_input_helper.before_helpers = before_helpers;
    _bug51_input_helper.magic_value = 0x5626e5a6;
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
static volatile __attribute__((used)) int _bug52_first_part;
static void _bug52_pattern_clean(void)
{
    pthread_mutex_init(&_bug52_mutex1, NULL);
    _bug52_first_part = -1;
}
static __attribute__((optimize(0))) int _bug52_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug52_mutex1);
        if (_bug52_first_part == -1)
            _bug52_first_part = part;
        else
            _bug52_first_part = -1;
        pthread_mutex_unlock(&_bug52_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug52_mutex1);
        if (_bug52_first_part == -1) {
            _bug52_first_part = part;
            trigger = 1;
        }
        else
            _bug52_first_part = -1;
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
    static const uint32_t indices[] = {0x3853c};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x2de6c72d, 0xfb58c3ae};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x4dbdbb9};
    static racebench_before_helper before_helpers[3] = {};
    _bug52_input_helper.interleave_num = 3;
    _bug52_input_helper.index_num = 1;
    _bug52_input_helper.indices = indices;
    _bug52_input_helper.sizes = sizes;
    _bug52_input_helper.pads = pads;
    _bug52_input_helper.before_parts = before_parts;
    _bug52_input_helper.mapped_inputs = mapped_inputs;
    _bug52_input_helper.before_helpers = before_helpers;
    _bug52_input_helper.magic_value = 0x2e1b6694;
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
static volatile __attribute__((used)) int _bug53_released;
static void _bug53_pattern_clean(void)
{
    pthread_mutex_init(&_bug53_mutex1, NULL);
    _bug53_first_part = -1;
    _bug53_released = -1;
}
static __attribute__((optimize(0))) int _bug53_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug53_mutex1) == 0)
                break;
            if (_bug53_released == 0) {
                _bug53_first_part = -1;
                _bug53_released = -1;
                return 1;
            }
        }
        if (_bug53_first_part == -1)
            _bug53_first_part = part;
        else {
            _bug53_first_part = -1;
            while (_bug53_released == -1) {}
            _bug53_released = -1;
        }
        pthread_mutex_unlock(&_bug53_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug53_mutex1);
        if (_bug53_first_part == -1) {
            _bug53_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug53_mutex1);
                _bug53_released = 1;
            }
            else {
                _bug53_released = 0;
                while (_bug53_released != -1) {}
                pthread_mutex_unlock(&_bug53_mutex1);
                return 1;
            }
        }
        else {
            _bug53_first_part = -1;
            pthread_mutex_unlock(&_bug53_mutex1);
        }
    }
    return 0;
}

static void _bug53_pattern_clean(void);
static int _bug53_pattern(int part);
static racebench_bug_helper _bug53_bug_helper;
static racebench_input_helper _bug53_input_helper;
__attribute__((constructor))
static void _bug53_input_helper_init(void)
{
    static const uint32_t indices[] = {0x548e};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xf4521277};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182c7e};
    static racebench_before_helper before_helpers[2] = {};
    _bug53_input_helper.interleave_num = 2;
    _bug53_input_helper.index_num = 1;
    _bug53_input_helper.indices = indices;
    _bug53_input_helper.sizes = sizes;
    _bug53_input_helper.pads = pads;
    _bug53_input_helper.before_parts = before_parts;
    _bug53_input_helper.mapped_inputs = mapped_inputs;
    _bug53_input_helper.before_helpers = before_helpers;
    _bug53_input_helper.magic_value = 0x956a3ef5;
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
static pthread_cond_t _bug54_cond1;
static volatile __attribute__((used)) int* volatile _bug54_ptr;
static volatile __attribute__((used)) int _bug54_val;
static volatile __attribute__((used)) int _bug54_pred;
static void _bug54_pattern_clean(void)
{
    pthread_mutex_init(&_bug54_mutex1, NULL);
    pthread_cond_init(&_bug54_cond1, NULL);
    _bug54_ptr = NULL;
    _bug54_val = 0;
    _bug54_pred = 0;
}
static __attribute__((optimize(0))) int _bug54_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug54_pred = 0;
        pthread_cond_signal(&_bug54_cond1);
        pthread_mutex_lock(&_bug54_mutex1);
        _bug54_ptr = &_bug54_val;
        _bug54_pred = 1;
        pthread_mutex_unlock(&_bug54_mutex1);
        pthread_cond_signal(&_bug54_cond1);
    }
    else {
        pthread_mutex_lock(&_bug54_mutex1);
        if (!_bug54_pred)
            pthread_cond_wait(&_bug54_cond1, &_bug54_mutex1);
        if (_bug54_ptr == NULL)
            trigger = 1;
        _bug54_ptr = NULL;
        _bug54_pred = 0;
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
    static const uint32_t indices[] = {0x2855f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x34022caf};
    static racebench_before_helper before_helpers[0] = {};
    _bug54_input_helper.interleave_num = 0;
    _bug54_input_helper.index_num = 1;
    _bug54_input_helper.indices = indices;
    _bug54_input_helper.sizes = sizes;
    _bug54_input_helper.pads = pads;
    _bug54_input_helper.before_parts = before_parts;
    _bug54_input_helper.mapped_inputs = mapped_inputs;
    _bug54_input_helper.before_helpers = before_helpers;
    _bug54_input_helper.magic_value = 0x34022caf;
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
static volatile __attribute__((used)) int _bug55_ok;
static volatile __attribute__((used)) int* volatile _bug55_ptr;
static volatile __attribute__((used)) int _bug55_val;
static void _bug55_pattern_clean(void)
{
    _bug55_ok = 0;
    _bug55_ptr = NULL;
    _bug55_val = 0;
}
static __attribute__((optimize(0))) int _bug55_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug55_ok = 1;
        __sync_synchronize();
        _bug55_ptr = &_bug55_val;
    }
    else {
        while (!_bug55_ok)
            pthread_yield();
        if (_bug55_ptr == NULL)
            trigger = 1;
        _bug55_ptr = NULL;
        _bug55_ok = 0;
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
    static const uint32_t indices[] = {0x2d30f};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x839288d1, 0xbe7d1ef9};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xe0a55b6a};
    static racebench_before_helper before_helpers[3] = {};
    _bug55_input_helper.interleave_num = 3;
    _bug55_input_helper.index_num = 1;
    _bug55_input_helper.indices = indices;
    _bug55_input_helper.sizes = sizes;
    _bug55_input_helper.pads = pads;
    _bug55_input_helper.before_parts = before_parts;
    _bug55_input_helper.mapped_inputs = mapped_inputs;
    _bug55_input_helper.before_helpers = before_helpers;
    _bug55_input_helper.magic_value = 0x22b50334;
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
    static const uint32_t indices[] = {0x6a6};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0x38667914};
    static racebench_before_helper before_helpers[0] = {};
    _bug56_input_helper.interleave_num = 0;
    _bug56_input_helper.index_num = 1;
    _bug56_input_helper.indices = indices;
    _bug56_input_helper.sizes = sizes;
    _bug56_input_helper.pads = pads;
    _bug56_input_helper.before_parts = before_parts;
    _bug56_input_helper.mapped_inputs = mapped_inputs;
    _bug56_input_helper.before_helpers = before_helpers;
    _bug56_input_helper.magic_value = 0x38667914;
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
    static const uint32_t indices[] = {0x19e06};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug57_input_helper.interleave_num = 1;
    _bug57_input_helper.index_num = 1;
    _bug57_input_helper.indices = indices;
    _bug57_input_helper.sizes = sizes;
    _bug57_input_helper.pads = pads;
    _bug57_input_helper.before_parts = before_parts;
    _bug57_input_helper.mapped_inputs = mapped_inputs;
    _bug57_input_helper.before_helpers = before_helpers;
    _bug57_input_helper.magic_value = 0xa1182cbe;
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
static volatile __attribute__((used)) int _bug58_ok;
static volatile __attribute__((used)) int* volatile _bug58_ptr;
static volatile __attribute__((used)) int _bug58_val;
static void _bug58_pattern_clean(void)
{
    _bug58_ok = 0;
    _bug58_ptr = NULL;
    _bug58_val = 0;
}
static __attribute__((optimize(0))) int _bug58_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug58_ok = 1;
        __sync_synchronize();
        _bug58_ptr = &_bug58_val;
    }
    else {
        while (!_bug58_ok)
            pthread_yield();
        if (_bug58_ptr == NULL)
            trigger = 1;
        _bug58_ptr = NULL;
        _bug58_ok = 0;
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
    static const uint32_t indices[] = {0x249ba};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x1a2af93};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xc645e96c};
    static racebench_before_helper before_helpers[2] = {};
    _bug58_input_helper.interleave_num = 2;
    _bug58_input_helper.index_num = 1;
    _bug58_input_helper.indices = indices;
    _bug58_input_helper.sizes = sizes;
    _bug58_input_helper.pads = pads;
    _bug58_input_helper.before_parts = before_parts;
    _bug58_input_helper.mapped_inputs = mapped_inputs;
    _bug58_input_helper.before_helpers = before_helpers;
    _bug58_input_helper.magic_value = 0xc7e898ff;
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
static volatile __attribute__((used)) int* volatile _bug59_ptr;
static volatile __attribute__((used)) int _bug59_val;
static void _bug59_pattern_clean(void)
{
    pthread_mutex_init(&_bug59_mutex1, NULL);
    _bug59_ptr = NULL;
    _bug59_val = 0;
}
static __attribute__((optimize(0))) int _bug59_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug59_mutex1);
        _bug59_ptr = &_bug59_val;
        pthread_mutex_unlock(&_bug59_mutex1);
        pthread_mutex_lock(&_bug59_mutex1);
        if (_bug59_ptr == NULL)
            trigger = 1;
        _bug59_ptr = NULL;
        pthread_mutex_unlock(&_bug59_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug59_mutex1);
        _bug59_ptr = &_bug59_val;
        if (_bug59_ptr == NULL)
            trigger = 1;
        _bug59_ptr = NULL;
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
    static const uint32_t indices[] = {0x9832};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa3425f0f};
    static racebench_before_helper before_helpers[0] = {};
    _bug59_input_helper.interleave_num = 0;
    _bug59_input_helper.index_num = 1;
    _bug59_input_helper.indices = indices;
    _bug59_input_helper.sizes = sizes;
    _bug59_input_helper.pads = pads;
    _bug59_input_helper.before_parts = before_parts;
    _bug59_input_helper.mapped_inputs = mapped_inputs;
    _bug59_input_helper.before_helpers = before_helpers;
    _bug59_input_helper.magic_value = 0xa3425f0f;
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
    static const uint32_t indices[] = {0xfe58};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x710322a3, 0xa1597e06};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa119cdde};
    static racebench_before_helper before_helpers[3] = {};
    _bug60_input_helper.interleave_num = 3;
    _bug60_input_helper.index_num = 1;
    _bug60_input_helper.indices = indices;
    _bug60_input_helper.sizes = sizes;
    _bug60_input_helper.pads = pads;
    _bug60_input_helper.before_parts = before_parts;
    _bug60_input_helper.mapped_inputs = mapped_inputs;
    _bug60_input_helper.before_helpers = before_helpers;
    _bug60_input_helper.magic_value = 0xb3766e87;
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
    static const uint32_t indices[] = {0x124b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0xbf34175};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1182f3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug61_input_helper.interleave_num = 2;
    _bug61_input_helper.index_num = 1;
    _bug61_input_helper.indices = indices;
    _bug61_input_helper.sizes = sizes;
    _bug61_input_helper.pads = pads;
    _bug61_input_helper.before_parts = before_parts;
    _bug61_input_helper.mapped_inputs = mapped_inputs;
    _bug61_input_helper.before_helpers = before_helpers;
    _bug61_input_helper.magic_value = 0xad0b70b3;
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
    static const uint32_t indices[] = {0x20203};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xdbbead87};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0x68cd5c20};
    static racebench_before_helper before_helpers[2] = {};
    _bug62_input_helper.interleave_num = 2;
    _bug62_input_helper.index_num = 1;
    _bug62_input_helper.indices = indices;
    _bug62_input_helper.sizes = sizes;
    _bug62_input_helper.pads = pads;
    _bug62_input_helper.before_parts = before_parts;
    _bug62_input_helper.mapped_inputs = mapped_inputs;
    _bug62_input_helper.before_helpers = before_helpers;
    _bug62_input_helper.magic_value = 0x448c09a7;
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
    static const uint32_t indices[] = {0x3c8d4};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x2845dbff};
    static racebench_before_helper before_helpers[1] = {};
    _bug63_input_helper.interleave_num = 1;
    _bug63_input_helper.index_num = 1;
    _bug63_input_helper.indices = indices;
    _bug63_input_helper.sizes = sizes;
    _bug63_input_helper.pads = pads;
    _bug63_input_helper.before_parts = before_parts;
    _bug63_input_helper.mapped_inputs = mapped_inputs;
    _bug63_input_helper.before_helpers = before_helpers;
    _bug63_input_helper.magic_value = 0x2845dbff;
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
    static const uint32_t indices[] = {0x2a0f3};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115575e};
    static racebench_before_helper before_helpers[0] = {};
    _bug64_input_helper.interleave_num = 0;
    _bug64_input_helper.index_num = 1;
    _bug64_input_helper.indices = indices;
    _bug64_input_helper.sizes = sizes;
    _bug64_input_helper.pads = pads;
    _bug64_input_helper.before_parts = before_parts;
    _bug64_input_helper.mapped_inputs = mapped_inputs;
    _bug64_input_helper.before_helpers = before_helpers;
    _bug64_input_helper.magic_value = 0xa115575e;
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
static volatile __attribute__((used)) int _bug65_arr[3];
static volatile __attribute__((used)) int _bug65_val;
static volatile __attribute__((used)) int _bug65_pred;
static void _bug65_pattern_clean(void)
{
    _bug65_arr[0] = 0, _bug65_arr[1] = 2, _bug65_arr[2] = 1;
    _bug65_val = 0;
    _bug65_pred = 0;
}
static __attribute__((optimize(0))) int _bug65_pattern(int part)
{
    int trigger = 0;
    _bug65_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug65_val += _bug65_arr[i];
        }
        __sync_synchronize();
        while (!_bug65_pred)
            pthread_yield();
        trigger = _bug65_val != 3;
        _bug65_pred = 0;
        _bug65_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug65_val += _bug65_arr[i];
        }
        __sync_synchronize();
        _bug65_pred = 1;
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
    static const uint32_t indices[] = {0x28f8};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x192c2c8e};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xc6b6d24f};
    static racebench_before_helper before_helpers[2] = {};
    _bug65_input_helper.interleave_num = 2;
    _bug65_input_helper.index_num = 1;
    _bug65_input_helper.indices = indices;
    _bug65_input_helper.sizes = sizes;
    _bug65_input_helper.pads = pads;
    _bug65_input_helper.before_parts = before_parts;
    _bug65_input_helper.mapped_inputs = mapped_inputs;
    _bug65_input_helper.before_helpers = before_helpers;
    _bug65_input_helper.magic_value = 0xdfe2fedd;
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
    static const uint32_t indices[] = {0x1d940};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x5f150701};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118313e};
    static racebench_before_helper before_helpers[2] = {};
    _bug66_input_helper.interleave_num = 2;
    _bug66_input_helper.index_num = 1;
    _bug66_input_helper.indices = indices;
    _bug66_input_helper.sizes = sizes;
    _bug66_input_helper.pads = pads;
    _bug66_input_helper.before_parts = before_parts;
    _bug66_input_helper.mapped_inputs = mapped_inputs;
    _bug66_input_helper.before_helpers = before_helpers;
    _bug66_input_helper.magic_value = 0x2d383f;
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
    static const uint32_t indices[] = {0x30f46};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x516d6c00};
    static const uint8_t before_parts[] = {0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182e3e};
    static racebench_before_helper before_helpers[2] = {};
    _bug67_input_helper.interleave_num = 2;
    _bug67_input_helper.index_num = 1;
    _bug67_input_helper.indices = indices;
    _bug67_input_helper.sizes = sizes;
    _bug67_input_helper.pads = pads;
    _bug67_input_helper.before_parts = before_parts;
    _bug67_input_helper.mapped_inputs = mapped_inputs;
    _bug67_input_helper.before_helpers = before_helpers;
    _bug67_input_helper.magic_value = 0xf2859a3e;
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
    static const uint32_t indices[] = {0x36b94};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xe495a49b};
    static racebench_before_helper before_helpers[0] = {};
    _bug68_input_helper.interleave_num = 0;
    _bug68_input_helper.index_num = 1;
    _bug68_input_helper.indices = indices;
    _bug68_input_helper.sizes = sizes;
    _bug68_input_helper.pads = pads;
    _bug68_input_helper.before_parts = before_parts;
    _bug68_input_helper.mapped_inputs = mapped_inputs;
    _bug68_input_helper.before_helpers = before_helpers;
    _bug68_input_helper.magic_value = 0xe495a49b;
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
static pthread_mutex_t _bug69_mutex2;
static void _bug69_pattern_clean(void)
{
    pthread_mutex_init(&_bug69_mutex1, NULL);
    pthread_mutex_init(&_bug69_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug69_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug69_mutex1);
        if (pthread_mutex_trylock(&_bug69_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug69_mutex2);
        pthread_mutex_unlock(&_bug69_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug69_mutex2);
        if (pthread_mutex_trylock(&_bug69_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug69_mutex1);
        pthread_mutex_unlock(&_bug69_mutex2);
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
    static const uint32_t indices[] = {0x2ce5b};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182ede};
    static racebench_before_helper before_helpers[0] = {};
    _bug69_input_helper.interleave_num = 0;
    _bug69_input_helper.index_num = 1;
    _bug69_input_helper.indices = indices;
    _bug69_input_helper.sizes = sizes;
    _bug69_input_helper.pads = pads;
    _bug69_input_helper.before_parts = before_parts;
    _bug69_input_helper.mapped_inputs = mapped_inputs;
    _bug69_input_helper.before_helpers = before_helpers;
    _bug69_input_helper.magic_value = 0xa1182ede;
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
static volatile __attribute__((used)) int _bug70_val;
static void _bug70_pattern_clean(void)
{
    pthread_mutex_init(&_bug70_mutex1, NULL);
    _bug70_val = 0;
}
static __attribute__((optimize(0))) int _bug70_pattern(int part)
{
    int trigger = 0;
    volatile int *ptr = &_bug70_val;
    if (part == 1) {
        pthread_mutex_lock(&_bug70_mutex1);
        int new_val = *ptr + 1;
        *ptr = new_val;
        __sync_synchronize();
        if (*ptr != new_val)
            trigger = 1;
        pthread_mutex_unlock(&_bug70_mutex1);
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

static void _bug70_pattern_clean(void);
static int _bug70_pattern(int part);
static racebench_bug_helper _bug70_bug_helper;
static racebench_input_helper _bug70_input_helper;
__attribute__((constructor))
static void _bug70_input_helper_init(void)
{
    static const uint32_t indices[] = {0x35e73};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa115a29e};
    static racebench_before_helper before_helpers[0] = {};
    _bug70_input_helper.interleave_num = 0;
    _bug70_input_helper.index_num = 1;
    _bug70_input_helper.indices = indices;
    _bug70_input_helper.sizes = sizes;
    _bug70_input_helper.pads = pads;
    _bug70_input_helper.before_parts = before_parts;
    _bug70_input_helper.mapped_inputs = mapped_inputs;
    _bug70_input_helper.before_helpers = before_helpers;
    _bug70_input_helper.magic_value = 0xa115a29e;
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
static void _bug71_pattern_clean(void)
{
    pthread_mutex_init(&_bug71_mutex1, NULL);
    _bug71_first_part = -1;
}
static __attribute__((optimize(0))) int _bug71_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug71_mutex1);
        if (_bug71_first_part == -1)
            _bug71_first_part = part;
        else
            _bug71_first_part = -1;
        pthread_mutex_unlock(&_bug71_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug71_mutex1);
        if (_bug71_first_part == -1) {
            _bug71_first_part = part;
            trigger = 1;
        }
        else
            _bug71_first_part = -1;
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
    static const uint32_t indices[] = {0x257f7};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa112027e};
    static racebench_before_helper before_helpers[0] = {};
    _bug71_input_helper.interleave_num = 0;
    _bug71_input_helper.index_num = 1;
    _bug71_input_helper.indices = indices;
    _bug71_input_helper.sizes = sizes;
    _bug71_input_helper.pads = pads;
    _bug71_input_helper.before_parts = before_parts;
    _bug71_input_helper.mapped_inputs = mapped_inputs;
    _bug71_input_helper.before_helpers = before_helpers;
    _bug71_input_helper.magic_value = 0xa112027e;
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
static volatile __attribute__((used)) int _bug72_arr[3];
static volatile __attribute__((used)) int _bug72_val;
static volatile __attribute__((used)) int _bug72_pred;
static void _bug72_pattern_clean(void)
{
    _bug72_arr[0] = 0, _bug72_arr[1] = 2, _bug72_arr[2] = 1;
    _bug72_val = 0;
    _bug72_pred = 0;
}
static __attribute__((optimize(0))) int _bug72_pattern(int part)
{
    int trigger = 0;
    _bug72_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug72_val += _bug72_arr[i];
        }
        __sync_synchronize();
        while (!_bug72_pred)
            pthread_yield();
        trigger = _bug72_val != 3;
        _bug72_pred = 0;
        _bug72_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug72_val += _bug72_arr[i];
        }
        __sync_synchronize();
        _bug72_pred = 1;
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
    static const uint32_t indices[] = {0x2ea1a};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1182cee};
    static racebench_before_helper before_helpers[1] = {};
    _bug72_input_helper.interleave_num = 1;
    _bug72_input_helper.index_num = 1;
    _bug72_input_helper.indices = indices;
    _bug72_input_helper.sizes = sizes;
    _bug72_input_helper.pads = pads;
    _bug72_input_helper.before_parts = before_parts;
    _bug72_input_helper.mapped_inputs = mapped_inputs;
    _bug72_input_helper.before_helpers = before_helpers;
    _bug72_input_helper.magic_value = 0xa1182cee;
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
    static const uint32_t indices[] = {0x43f6e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xd804cb56, 0x1968535};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa113ae1e};
    static racebench_before_helper before_helpers[3] = {};
    _bug73_input_helper.interleave_num = 3;
    _bug73_input_helper.index_num = 1;
    _bug73_input_helper.indices = indices;
    _bug73_input_helper.sizes = sizes;
    _bug73_input_helper.pads = pads;
    _bug73_input_helper.before_parts = before_parts;
    _bug73_input_helper.mapped_inputs = mapped_inputs;
    _bug73_input_helper.before_helpers = before_helpers;
    _bug73_input_helper.magic_value = 0x7aaefea9;
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
    static const uint32_t indices[] = {0x20ce5};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa118278e};
    static racebench_before_helper before_helpers[0] = {};
    _bug74_input_helper.interleave_num = 0;
    _bug74_input_helper.index_num = 1;
    _bug74_input_helper.indices = indices;
    _bug74_input_helper.sizes = sizes;
    _bug74_input_helper.pads = pads;
    _bug74_input_helper.before_parts = before_parts;
    _bug74_input_helper.mapped_inputs = mapped_inputs;
    _bug74_input_helper.before_helpers = before_helpers;
    _bug74_input_helper.magic_value = 0xa118278e;
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
    static const uint32_t indices[] = {0x6d94};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182e1e};
    static racebench_before_helper before_helpers[0] = {};
    _bug75_input_helper.interleave_num = 0;
    _bug75_input_helper.index_num = 1;
    _bug75_input_helper.indices = indices;
    _bug75_input_helper.sizes = sizes;
    _bug75_input_helper.pads = pads;
    _bug75_input_helper.before_parts = before_parts;
    _bug75_input_helper.mapped_inputs = mapped_inputs;
    _bug75_input_helper.before_helpers = before_helpers;
    _bug75_input_helper.magic_value = 0xa1182e1e;
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
    static const uint32_t indices[] = {0x1225b};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x759deedb};
    static const uint8_t before_parts[] = {0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1145c8e};
    static racebench_before_helper before_helpers[2] = {};
    _bug76_input_helper.interleave_num = 2;
    _bug76_input_helper.index_num = 1;
    _bug76_input_helper.indices = indices;
    _bug76_input_helper.sizes = sizes;
    _bug76_input_helper.pads = pads;
    _bug76_input_helper.before_parts = before_parts;
    _bug76_input_helper.mapped_inputs = mapped_inputs;
    _bug76_input_helper.before_helpers = before_helpers;
    _bug76_input_helper.magic_value = 0x16b24b69;
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
static pthread_mutex_t _bug77_mutex2;
static void _bug77_pattern_clean(void)
{
    pthread_mutex_init(&_bug77_mutex1, NULL);
    pthread_mutex_init(&_bug77_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug77_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug77_mutex1);
        if (pthread_mutex_trylock(&_bug77_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug77_mutex2);
        pthread_mutex_unlock(&_bug77_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug77_mutex2);
        if (pthread_mutex_trylock(&_bug77_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug77_mutex1);
        pthread_mutex_unlock(&_bug77_mutex2);
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
    static const uint32_t indices[] = {0x22aba};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1143efe};
    static racebench_before_helper before_helpers[0] = {};
    _bug77_input_helper.interleave_num = 0;
    _bug77_input_helper.index_num = 1;
    _bug77_input_helper.indices = indices;
    _bug77_input_helper.sizes = sizes;
    _bug77_input_helper.pads = pads;
    _bug77_input_helper.before_parts = before_parts;
    _bug77_input_helper.mapped_inputs = mapped_inputs;
    _bug77_input_helper.before_helpers = before_helpers;
    _bug77_input_helper.magic_value = 0xa1143efe;
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
static volatile __attribute__((used)) int _bug78_arr[3];
static volatile __attribute__((used)) int _bug78_val;
static volatile __attribute__((used)) int _bug78_pred;
static void _bug78_pattern_clean(void)
{
    _bug78_arr[0] = 0, _bug78_arr[1] = 2, _bug78_arr[2] = 1;
    _bug78_val = 0;
    _bug78_pred = 0;
}
static __attribute__((optimize(0))) int _bug78_pattern(int part)
{
    int trigger = 0;
    _bug78_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug78_val += _bug78_arr[i];
        }
        __sync_synchronize();
        while (!_bug78_pred)
            pthread_yield();
        trigger = _bug78_val != 3;
        _bug78_pred = 0;
        _bug78_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug78_val += _bug78_arr[i];
        }
        __sync_synchronize();
        _bug78_pred = 1;
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
    static const uint32_t indices[] = {0x1957};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x9a9a009, 0xf99049e6};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa1182f5e};
    static racebench_before_helper before_helpers[3] = {};
    _bug78_input_helper.interleave_num = 3;
    _bug78_input_helper.index_num = 1;
    _bug78_input_helper.indices = indices;
    _bug78_input_helper.sizes = sizes;
    _bug78_input_helper.pads = pads;
    _bug78_input_helper.before_parts = before_parts;
    _bug78_input_helper.mapped_inputs = mapped_inputs;
    _bug78_input_helper.before_helpers = before_helpers;
    _bug78_input_helper.magic_value = 0xa452194d;
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
    static const uint32_t indices[] = {0x31d22};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x66676bd2};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa116e98e};
    static racebench_before_helper before_helpers[2] = {};
    _bug79_input_helper.interleave_num = 2;
    _bug79_input_helper.index_num = 1;
    _bug79_input_helper.indices = indices;
    _bug79_input_helper.sizes = sizes;
    _bug79_input_helper.pads = pads;
    _bug79_input_helper.before_parts = before_parts;
    _bug79_input_helper.mapped_inputs = mapped_inputs;
    _bug79_input_helper.before_helpers = before_helpers;
    _bug79_input_helper.magic_value = 0x77e5560;
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
    static const uint32_t indices[] = {0x46d90};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa119dd7e};
    static racebench_before_helper before_helpers[0] = {};
    _bug80_input_helper.interleave_num = 0;
    _bug80_input_helper.index_num = 1;
    _bug80_input_helper.indices = indices;
    _bug80_input_helper.sizes = sizes;
    _bug80_input_helper.pads = pads;
    _bug80_input_helper.before_parts = before_parts;
    _bug80_input_helper.mapped_inputs = mapped_inputs;
    _bug80_input_helper.before_helpers = before_helpers;
    _bug80_input_helper.magic_value = 0xa119dd7e;
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
static void _bug81_pattern_clean(void)
{
    pthread_mutex_init(&_bug81_mutex1, NULL);
    _bug81_first_part = -1;
}
static __attribute__((optimize(0))) int _bug81_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug81_mutex1);
        if (_bug81_first_part == -1)
            _bug81_first_part = part;
        else
            _bug81_first_part = -1;
        pthread_mutex_unlock(&_bug81_mutex1);
    }
    else {
        usleep(1000);
        pthread_mutex_lock(&_bug81_mutex1);
        if (_bug81_first_part == -1) {
            _bug81_first_part = part;
            trigger = 1;
        }
        else
            _bug81_first_part = -1;
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
    static const uint32_t indices[] = {0x23ce9};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa112cb3e};
    static racebench_before_helper before_helpers[1] = {};
    _bug81_input_helper.interleave_num = 1;
    _bug81_input_helper.index_num = 1;
    _bug81_input_helper.indices = indices;
    _bug81_input_helper.sizes = sizes;
    _bug81_input_helper.pads = pads;
    _bug81_input_helper.before_parts = before_parts;
    _bug81_input_helper.mapped_inputs = mapped_inputs;
    _bug81_input_helper.before_helpers = before_helpers;
    _bug81_input_helper.magic_value = 0xa112cb3e;
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
static volatile __attribute__((used)) int _bug82_arr[3];
static volatile __attribute__((used)) int _bug82_val;
static volatile __attribute__((used)) int _bug82_pred;
static void _bug82_pattern_clean(void)
{
    _bug82_arr[0] = 0, _bug82_arr[1] = 2, _bug82_arr[2] = 1;
    _bug82_val = 0;
    _bug82_pred = 0;
}
static __attribute__((optimize(0))) int _bug82_pattern(int part)
{
    int trigger = 0;
    _bug82_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug82_val += _bug82_arr[i];
        }
        __sync_synchronize();
        while (!_bug82_pred)
            pthread_yield();
        trigger = _bug82_val != 3;
        _bug82_pred = 0;
        _bug82_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug82_val += _bug82_arr[i];
        }
        __sync_synchronize();
        _bug82_pred = 1;
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
    static const uint32_t indices[] = {0x162c5};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa113882e};
    static racebench_before_helper before_helpers[1] = {};
    _bug82_input_helper.interleave_num = 1;
    _bug82_input_helper.index_num = 1;
    _bug82_input_helper.indices = indices;
    _bug82_input_helper.sizes = sizes;
    _bug82_input_helper.pads = pads;
    _bug82_input_helper.before_parts = before_parts;
    _bug82_input_helper.mapped_inputs = mapped_inputs;
    _bug82_input_helper.before_helpers = before_helpers;
    _bug82_input_helper.magic_value = 0xa113882e;
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
    static const uint32_t indices[] = {0x27151};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x54dad9e, 0xbfc3454b};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x1};
    static const uint32_t mapped_inputs[] = {0x3654228b};
    static racebench_before_helper before_helpers[3] = {};
    _bug83_input_helper.interleave_num = 3;
    _bug83_input_helper.index_num = 1;
    _bug83_input_helper.indices = indices;
    _bug83_input_helper.sizes = sizes;
    _bug83_input_helper.pads = pads;
    _bug83_input_helper.before_parts = before_parts;
    _bug83_input_helper.mapped_inputs = mapped_inputs;
    _bug83_input_helper.before_helpers = before_helpers;
    _bug83_input_helper.magic_value = 0xfb651574;
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
static volatile __attribute__((used)) int _bug84_arr[3];
static volatile __attribute__((used)) int _bug84_val;
static volatile __attribute__((used)) int _bug84_pred;
static void _bug84_pattern_clean(void)
{
    _bug84_arr[0] = 0, _bug84_arr[1] = 2, _bug84_arr[2] = 1;
    _bug84_val = 0;
    _bug84_pred = 0;
}
static __attribute__((optimize(0))) int _bug84_pattern(int part)
{
    int trigger = 0;
    _bug84_arr[part+1] = 2 - part;
    if (part == 1) {
        for (int i = 0; i < 3/2; ++i) {
            _bug84_val += _bug84_arr[i];
        }
        __sync_synchronize();
        while (!_bug84_pred)
            pthread_yield();
        trigger = _bug84_val != 3;
        _bug84_pred = 0;
        _bug84_val = 0;
    }
    else {
        for (int i = 3/2; i < 3; ++i) {
            _bug84_val += _bug84_arr[i];
        }
        __sync_synchronize();
        _bug84_pred = 1;
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
    static const uint32_t indices[] = {0x42a9d};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xdcf1cc50};
    static racebench_before_helper before_helpers[0] = {};
    _bug84_input_helper.interleave_num = 0;
    _bug84_input_helper.index_num = 1;
    _bug84_input_helper.indices = indices;
    _bug84_input_helper.sizes = sizes;
    _bug84_input_helper.pads = pads;
    _bug84_input_helper.before_parts = before_parts;
    _bug84_input_helper.mapped_inputs = mapped_inputs;
    _bug84_input_helper.before_helpers = before_helpers;
    _bug84_input_helper.magic_value = 0xdcf1cc50;
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
    static const uint32_t indices[] = {0x23d30};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x2c8380c8};
    static const uint8_t before_parts[] = {0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa111cdee};
    static racebench_before_helper before_helpers[2] = {};
    _bug85_input_helper.interleave_num = 2;
    _bug85_input_helper.index_num = 1;
    _bug85_input_helper.indices = indices;
    _bug85_input_helper.sizes = sizes;
    _bug85_input_helper.pads = pads;
    _bug85_input_helper.before_parts = before_parts;
    _bug85_input_helper.mapped_inputs = mapped_inputs;
    _bug85_input_helper.before_helpers = before_helpers;
    _bug85_input_helper.magic_value = 0xcd954eb6;
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
static pthread_cond_t _bug86_cond1;
static volatile __attribute__((used)) int _bug86_pred;
static volatile __attribute__((used)) int _bug86_sent;
static void _bug86_pattern_clean(void)
{
    pthread_mutex_init(&_bug86_mutex1, NULL);
    pthread_cond_init(&_bug86_cond1, NULL);
    _bug86_pred = 0;
    _bug86_sent = 0;
}
static __attribute__((optimize(0))) int _bug86_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug86_mutex1);
        _bug86_pred = 1;
        pthread_cond_signal(&_bug86_cond1);
        _bug86_sent = 1;
        pthread_mutex_unlock(&_bug86_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug86_mutex1);
        if (_bug86_sent) {
            trigger = 1;
        }
        else {
            while (_bug86_pred == 0)
                pthread_cond_wait(&_bug86_cond1, &_bug86_mutex1);
        }
        _bug86_pred = 0;
        pthread_mutex_unlock(&_bug86_mutex1);
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
    static const uint32_t indices[] = {0x3b008};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0x90f52c8f};
    static racebench_before_helper before_helpers[1] = {};
    _bug86_input_helper.interleave_num = 1;
    _bug86_input_helper.index_num = 1;
    _bug86_input_helper.indices = indices;
    _bug86_input_helper.sizes = sizes;
    _bug86_input_helper.pads = pads;
    _bug86_input_helper.before_parts = before_parts;
    _bug86_input_helper.mapped_inputs = mapped_inputs;
    _bug86_input_helper.before_helpers = before_helpers;
    _bug86_input_helper.magic_value = 0x90f52c8f;
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
static volatile __attribute__((used)) int _bug87_ok;
static volatile __attribute__((used)) int* volatile _bug87_ptr;
static volatile __attribute__((used)) int _bug87_val;
static void _bug87_pattern_clean(void)
{
    _bug87_ok = 0;
    _bug87_ptr = NULL;
    _bug87_val = 0;
}
static __attribute__((optimize(0))) int _bug87_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug87_ok = 1;
        __sync_synchronize();
        _bug87_ptr = &_bug87_val;
    }
    else {
        while (!_bug87_ok)
            pthread_yield();
        if (_bug87_ptr == NULL)
            trigger = 1;
        _bug87_ptr = NULL;
        _bug87_ok = 0;
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
    static const uint32_t indices[] = {0x2a52e};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0x795534c9};
    static const uint8_t before_parts[] = {0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xb75400fb};
    static racebench_before_helper before_helpers[2] = {};
    _bug87_input_helper.interleave_num = 2;
    _bug87_input_helper.index_num = 1;
    _bug87_input_helper.indices = indices;
    _bug87_input_helper.sizes = sizes;
    _bug87_input_helper.pads = pads;
    _bug87_input_helper.before_parts = before_parts;
    _bug87_input_helper.mapped_inputs = mapped_inputs;
    _bug87_input_helper.before_helpers = before_helpers;
    _bug87_input_helper.magic_value = 0x30a935c4;
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
static pthread_mutex_t _bug88_mutex2;
static void _bug88_pattern_clean(void)
{
    pthread_mutex_init(&_bug88_mutex1, NULL);
    pthread_mutex_init(&_bug88_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug88_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug88_mutex1);
        if (pthread_mutex_trylock(&_bug88_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug88_mutex2);
        pthread_mutex_unlock(&_bug88_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug88_mutex2);
        if (pthread_mutex_trylock(&_bug88_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug88_mutex1);
        pthread_mutex_unlock(&_bug88_mutex2);
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
    static const uint32_t indices[] = {0x15b4e};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa11d713e};
    static racebench_before_helper before_helpers[0] = {};
    _bug88_input_helper.interleave_num = 0;
    _bug88_input_helper.index_num = 1;
    _bug88_input_helper.indices = indices;
    _bug88_input_helper.sizes = sizes;
    _bug88_input_helper.pads = pads;
    _bug88_input_helper.before_parts = before_parts;
    _bug88_input_helper.mapped_inputs = mapped_inputs;
    _bug88_input_helper.before_helpers = before_helpers;
    _bug88_input_helper.magic_value = 0xa11d713e;
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
    static const uint32_t indices[] = {0x32358};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x7814b606, 0x90a476ae};
    static const uint8_t before_parts[] = {0x1, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1168fae};
    static racebench_before_helper before_helpers[3] = {};
    _bug89_input_helper.interleave_num = 3;
    _bug89_input_helper.index_num = 1;
    _bug89_input_helper.indices = indices;
    _bug89_input_helper.sizes = sizes;
    _bug89_input_helper.pads = pads;
    _bug89_input_helper.before_parts = before_parts;
    _bug89_input_helper.mapped_inputs = mapped_inputs;
    _bug89_input_helper.before_helpers = before_helpers;
    _bug89_input_helper.magic_value = 0xa9cfbc62;
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
static volatile __attribute__((used)) int _bug90_pred;
static volatile __attribute__((used)) int _bug90_sent;
static void _bug90_pattern_clean(void)
{
    pthread_mutex_init(&_bug90_mutex1, NULL);
    pthread_cond_init(&_bug90_cond1, NULL);
    _bug90_pred = 0;
    _bug90_sent = 0;
}
static __attribute__((optimize(0))) int _bug90_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug90_mutex1);
        _bug90_pred = 1;
        pthread_cond_signal(&_bug90_cond1);
        _bug90_sent = 1;
        pthread_mutex_unlock(&_bug90_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug90_mutex1);
        if (_bug90_sent) {
            trigger = 1;
        }
        else {
            while (_bug90_pred == 0)
                pthread_cond_wait(&_bug90_cond1, &_bug90_mutex1);
        }
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
    static const uint32_t indices[] = {0x23281};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x1};
    static const uint32_t mapped_inputs[] = {0xa1153bfe};
    static racebench_before_helper before_helpers[1] = {};
    _bug90_input_helper.interleave_num = 1;
    _bug90_input_helper.index_num = 1;
    _bug90_input_helper.indices = indices;
    _bug90_input_helper.sizes = sizes;
    _bug90_input_helper.pads = pads;
    _bug90_input_helper.before_parts = before_parts;
    _bug90_input_helper.mapped_inputs = mapped_inputs;
    _bug90_input_helper.before_helpers = before_helpers;
    _bug90_input_helper.magic_value = 0xa1153bfe;
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
    static const uint32_t indices[] = {0x1766f};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1182b7e};
    static racebench_before_helper before_helpers[1] = {};
    _bug91_input_helper.interleave_num = 1;
    _bug91_input_helper.index_num = 1;
    _bug91_input_helper.indices = indices;
    _bug91_input_helper.sizes = sizes;
    _bug91_input_helper.pads = pads;
    _bug91_input_helper.before_parts = before_parts;
    _bug91_input_helper.mapped_inputs = mapped_inputs;
    _bug91_input_helper.before_helpers = before_helpers;
    _bug91_input_helper.magic_value = 0xa1182b7e;
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
static volatile __attribute__((used)) int _bug92_released;
static void _bug92_pattern_clean(void)
{
    pthread_mutex_init(&_bug92_mutex1, NULL);
    _bug92_first_part = -1;
    _bug92_released = -1;
}
static __attribute__((optimize(0))) int _bug92_pattern(int part)
{
    if (part == 1) {
        while (1) {
            if (pthread_mutex_trylock(&_bug92_mutex1) == 0)
                break;
            if (_bug92_released == 0) {
                _bug92_first_part = -1;
                _bug92_released = -1;
                return 1;
            }
        }
        if (_bug92_first_part == -1)
            _bug92_first_part = part;
        else {
            _bug92_first_part = -1;
            while (_bug92_released == -1) {}
            _bug92_released = -1;
        }
        pthread_mutex_unlock(&_bug92_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug92_mutex1);
        if (_bug92_first_part == -1) {
            _bug92_first_part = part;
            if (time(NULL) % 10 != 0) {
                pthread_mutex_unlock(&_bug92_mutex1);
                _bug92_released = 1;
            }
            else {
                _bug92_released = 0;
                while (_bug92_released != -1) {}
                pthread_mutex_unlock(&_bug92_mutex1);
                return 1;
            }
        }
        else {
            _bug92_first_part = -1;
            pthread_mutex_unlock(&_bug92_mutex1);
        }
    }
    return 0;
}

static void _bug92_pattern_clean(void);
static int _bug92_pattern(int part);
static racebench_bug_helper _bug92_bug_helper;
static racebench_input_helper _bug92_input_helper;
__attribute__((constructor))
static void _bug92_input_helper_init(void)
{
    static const uint32_t indices[] = {0x42ff3};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182ede};
    static racebench_before_helper before_helpers[0] = {};
    _bug92_input_helper.interleave_num = 0;
    _bug92_input_helper.index_num = 1;
    _bug92_input_helper.indices = indices;
    _bug92_input_helper.sizes = sizes;
    _bug92_input_helper.pads = pads;
    _bug92_input_helper.before_parts = before_parts;
    _bug92_input_helper.mapped_inputs = mapped_inputs;
    _bug92_input_helper.before_helpers = before_helpers;
    _bug92_input_helper.magic_value = 0xa1182ede;
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
static pthread_cond_t _bug93_cond1;
static volatile __attribute__((used)) int* volatile _bug93_ptr;
static volatile __attribute__((used)) int _bug93_val;
static volatile __attribute__((used)) int _bug93_pred;
static void _bug93_pattern_clean(void)
{
    pthread_mutex_init(&_bug93_mutex1, NULL);
    pthread_cond_init(&_bug93_cond1, NULL);
    _bug93_ptr = NULL;
    _bug93_val = 0;
    _bug93_pred = 0;
}
static __attribute__((optimize(0))) int _bug93_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug93_pred = 0;
        pthread_cond_signal(&_bug93_cond1);
        pthread_mutex_lock(&_bug93_mutex1);
        _bug93_ptr = &_bug93_val;
        _bug93_pred = 1;
        pthread_mutex_unlock(&_bug93_mutex1);
        pthread_cond_signal(&_bug93_cond1);
    }
    else {
        pthread_mutex_lock(&_bug93_mutex1);
        if (!_bug93_pred)
            pthread_cond_wait(&_bug93_cond1, &_bug93_mutex1);
        if (_bug93_ptr == NULL)
            trigger = 1;
        _bug93_ptr = NULL;
        _bug93_pred = 0;
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
    static const uint32_t indices[] = {0x3c0cb};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb55fc48, 0xf1522a5a};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xc425bb1b};
    static racebench_before_helper before_helpers[3] = {};
    _bug93_input_helper.interleave_num = 3;
    _bug93_input_helper.index_num = 1;
    _bug93_input_helper.indices = indices;
    _bug93_input_helper.sizes = sizes;
    _bug93_input_helper.pads = pads;
    _bug93_input_helper.before_parts = before_parts;
    _bug93_input_helper.mapped_inputs = mapped_inputs;
    _bug93_input_helper.before_helpers = before_helpers;
    _bug93_input_helper.magic_value = 0xc0cde1bd;
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
static pthread_mutex_t _bug94_mutex2;
static void _bug94_pattern_clean(void)
{
    pthread_mutex_init(&_bug94_mutex1, NULL);
    pthread_mutex_init(&_bug94_mutex2, NULL);
}
static __attribute__((optimize(0))) int _bug94_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug94_mutex1);
        if (pthread_mutex_trylock(&_bug94_mutex2) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug94_mutex2);
        pthread_mutex_unlock(&_bug94_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug94_mutex2);
        if (pthread_mutex_trylock(&_bug94_mutex1) != 0)
            trigger = 1;
        else
            pthread_mutex_unlock(&_bug94_mutex1);
        pthread_mutex_unlock(&_bug94_mutex2);
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
    static const uint32_t indices[] = {0x3f039};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {0x618eecdd, 0xf9e339cb};
    static const uint8_t before_parts[] = {0x0, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0xa118216e};
    static racebench_before_helper before_helpers[3] = {};
    _bug94_input_helper.interleave_num = 3;
    _bug94_input_helper.index_num = 1;
    _bug94_input_helper.indices = indices;
    _bug94_input_helper.sizes = sizes;
    _bug94_input_helper.pads = pads;
    _bug94_input_helper.before_parts = before_parts;
    _bug94_input_helper.mapped_inputs = mapped_inputs;
    _bug94_input_helper.before_helpers = before_helpers;
    _bug94_input_helper.magic_value = 0xfc8a4816;
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
static volatile __attribute__((used)) int* volatile _bug95_ptr;
static volatile __attribute__((used)) int _bug95_val;
static volatile __attribute__((used)) int _bug95_pred;
static void _bug95_pattern_clean(void)
{
    pthread_mutex_init(&_bug95_mutex1, NULL);
    pthread_cond_init(&_bug95_cond1, NULL);
    _bug95_ptr = NULL;
    _bug95_val = 0;
    _bug95_pred = 0;
}
static __attribute__((optimize(0))) int _bug95_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug95_pred = 0;
        pthread_cond_signal(&_bug95_cond1);
        pthread_mutex_lock(&_bug95_mutex1);
        _bug95_ptr = &_bug95_val;
        _bug95_pred = 1;
        pthread_mutex_unlock(&_bug95_mutex1);
        pthread_cond_signal(&_bug95_cond1);
    }
    else {
        pthread_mutex_lock(&_bug95_mutex1);
        if (!_bug95_pred)
            pthread_cond_wait(&_bug95_cond1, &_bug95_mutex1);
        if (_bug95_ptr == NULL)
            trigger = 1;
        _bug95_ptr = NULL;
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
    static const uint32_t indices[] = {0x30387};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {0x0};
    static const uint32_t mapped_inputs[] = {0xa1158cbe};
    static racebench_before_helper before_helpers[1] = {};
    _bug95_input_helper.interleave_num = 1;
    _bug95_input_helper.index_num = 1;
    _bug95_input_helper.indices = indices;
    _bug95_input_helper.sizes = sizes;
    _bug95_input_helper.pads = pads;
    _bug95_input_helper.before_parts = before_parts;
    _bug95_input_helper.mapped_inputs = mapped_inputs;
    _bug95_input_helper.before_helpers = before_helpers;
    _bug95_input_helper.magic_value = 0xa1158cbe;
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
static volatile __attribute__((used)) int* volatile _bug96_ptr;
static volatile __attribute__((used)) int _bug96_val;
static void _bug96_pattern_clean(void)
{
    pthread_mutex_init(&_bug96_mutex1, NULL);
    _bug96_ptr = NULL;
    _bug96_val = 0;
}
static __attribute__((optimize(0))) int _bug96_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        pthread_mutex_lock(&_bug96_mutex1);
        _bug96_ptr = &_bug96_val;
        pthread_mutex_unlock(&_bug96_mutex1);
        pthread_mutex_lock(&_bug96_mutex1);
        if (_bug96_ptr == NULL)
            trigger = 1;
        _bug96_ptr = NULL;
        pthread_mutex_unlock(&_bug96_mutex1);
    }
    else {
        pthread_mutex_lock(&_bug96_mutex1);
        _bug96_ptr = &_bug96_val;
        if (_bug96_ptr == NULL)
            trigger = 1;
        _bug96_ptr = NULL;
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
    static const uint32_t indices[] = {0x9a75};
    static const uint8_t sizes[] = {0x1};
    static const uint32_t pads[] = {};
    static const uint8_t before_parts[] = {};
    static const uint32_t mapped_inputs[] = {0xa1182c3e};
    static racebench_before_helper before_helpers[0] = {};
    _bug96_input_helper.interleave_num = 0;
    _bug96_input_helper.index_num = 1;
    _bug96_input_helper.indices = indices;
    _bug96_input_helper.sizes = sizes;
    _bug96_input_helper.pads = pads;
    _bug96_input_helper.before_parts = before_parts;
    _bug96_input_helper.mapped_inputs = mapped_inputs;
    _bug96_input_helper.before_helpers = before_helpers;
    _bug96_input_helper.magic_value = 0xa1182c3e;
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
static volatile __attribute__((used)) int _bug97_ok;
static volatile __attribute__((used)) int* volatile _bug97_ptr;
static volatile __attribute__((used)) int _bug97_val;
static void _bug97_pattern_clean(void)
{
    _bug97_ok = 0;
    _bug97_ptr = NULL;
    _bug97_val = 0;
}
static __attribute__((optimize(0))) int _bug97_pattern(int part)
{
    int trigger = 0;
    if (part == 1) {
        _bug97_ok = 1;
        __sync_synchronize();
        _bug97_ptr = &_bug97_val;
    }
    else {
        while (!_bug97_ok)
            pthread_yield();
        if (_bug97_ptr == NULL)
            trigger = 1;
        _bug97_ptr = NULL;
        _bug97_ok = 0;
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
    static const uint32_t indices[] = {0x18fb5};
    static const uint8_t sizes[] = {0x4};
    static const uint32_t pads[] = {0xb60b8e71, 0x3e6821e6};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x0};
    static const uint32_t mapped_inputs[] = {0x10554fd0};
    static racebench_before_helper before_helpers[3] = {};
    _bug97_input_helper.interleave_num = 3;
    _bug97_input_helper.index_num = 1;
    _bug97_input_helper.indices = indices;
    _bug97_input_helper.sizes = sizes;
    _bug97_input_helper.pads = pads;
    _bug97_input_helper.before_parts = before_parts;
    _bug97_input_helper.mapped_inputs = mapped_inputs;
    _bug97_input_helper.before_helpers = before_helpers;
    _bug97_input_helper.magic_value = 0x4c90027;
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
    static const uint32_t indices[] = {0x18905};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0x3ad0e404, 0x424cf88e};
    static const uint8_t before_parts[] = {0x1, 0x0, 0x1};
    static const uint32_t mapped_inputs[] = {0xa115d8ae};
    static racebench_before_helper before_helpers[3] = {};
    _bug98_input_helper.interleave_num = 3;
    _bug98_input_helper.index_num = 1;
    _bug98_input_helper.indices = indices;
    _bug98_input_helper.sizes = sizes;
    _bug98_input_helper.pads = pads;
    _bug98_input_helper.before_parts = before_parts;
    _bug98_input_helper.mapped_inputs = mapped_inputs;
    _bug98_input_helper.before_helpers = before_helpers;
    _bug98_input_helper.magic_value = 0x1e33b540;
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
    static const uint32_t indices[] = {0x36a68};
    static const uint8_t sizes[] = {0x2};
    static const uint32_t pads[] = {0xf4c499c7, 0x56ac96d8};
    static const uint8_t before_parts[] = {0x0, 0x1, 0x0};
    static const uint32_t mapped_inputs[] = {0xa1150eee};
    static racebench_before_helper before_helpers[3] = {};
    _bug99_input_helper.interleave_num = 3;
    _bug99_input_helper.index_num = 1;
    _bug99_input_helper.indices = indices;
    _bug99_input_helper.sizes = sizes;
    _bug99_input_helper.pads = pads;
    _bug99_input_helper.before_parts = before_parts;
    _bug99_input_helper.mapped_inputs = mapped_inputs;
    _bug99_input_helper.before_helpers = before_helpers;
    _bug99_input_helper.magic_value = 0xec863f8d;
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
