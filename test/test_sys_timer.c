/*
MIT License

This file is part of cupkee project

Copyright (c) 2017 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <string.h>

#include "test.h"
#include <cupkee.h>

static int test_setup(void)
{
    cupkee_memory_desc_t desc = {64, 4};

    cupkee_memory_init(1, &desc);

    return 0;
}

static int test_clean(void)
{
    return 0;
}

static int v1[2], v2[2], v3[2], v4[2];

static void test_handle(int drop, void *param)
{
    int *pv = (int *) param;

    if (drop) {
        pv[1] += 1;
    } else {
        pv[0] += 1;
    }
}

static void test_wakeup(void)
{
    cupkee_timer_t *t1, *t2;

    cupkee_timer_init();
    _cupkee_systicks = 0;

    v1[0] = 0; v1[1] = 0;
    v2[0] = 0; v2[1] = 0;

    CU_ASSERT_FATAL((t1 = cupkee_timer_register(20, 1, test_handle, &v1)) != NULL);
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 0, test_handle, &v2)) != NULL);

    while (_cupkee_systicks < 10) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 0);
    CU_ASSERT(v1[1] == 0);
    CU_ASSERT(v2[0] == 0);
    CU_ASSERT(v2[1] == 0);

    while (_cupkee_systicks < 100) {
        cupkee_timer_sync(++_cupkee_systicks);
    }

    CU_ASSERT(v1[0] == 5);
    CU_ASSERT(v1[1] == 0);
    CU_ASSERT(v2[0] == 1);
    CU_ASSERT(v2[1] == 1);

    v2[0] = 0;
    v2[1] = 0;
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 0, test_handle, &v2)) != NULL);

    while (_cupkee_systicks < 130) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 6);
    CU_ASSERT(v1[1] == 0);
    CU_ASSERT(v2[0] == 1);
    CU_ASSERT(v2[1] == 1);

    cupkee_timer_unregister(t1);
    while (_cupkee_systicks < 200) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 6);
    CU_ASSERT(v1[1] == 1);
    CU_ASSERT(v2[0] == 1);
    CU_ASSERT(v2[1] == 1);
}

static void test_self_clear(void)
{
    cupkee_timer_t *t1, *t2, *t3, *t4;

    cupkee_timer_init();
    _cupkee_systicks = 0;

    v1[0] = 0; v1[1] = 0;
    v2[0] = 0; v2[1] = 0;
    v3[0] = 0; v3[1] = 0;
    v4[0] = 0; v4[1] = 0;

    CU_ASSERT_FATAL((t1 = cupkee_timer_register(20, 0, test_handle, &v1)) != NULL);
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 0, test_handle, &v2)) != NULL);
    CU_ASSERT_FATAL((t3 = cupkee_timer_register(40, 0, test_handle, &v3)) != NULL);
    CU_ASSERT_FATAL((t4 = cupkee_timer_register(50, 0, test_handle, &v4)) != NULL);

    while (_cupkee_systicks < 60) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 1 && v1[1] == 1);
    CU_ASSERT(v2[0] == 1 && v2[1] == 1);
    CU_ASSERT(v3[0] == 1 && v3[1] == 1);
    CU_ASSERT(v4[0] == 1 && v4[1] == 1);

    CU_ASSERT_FATAL((t1 = cupkee_timer_register(20, 0, test_handle, &v1)) != NULL);
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 0, test_handle, &v2)) != NULL);
    CU_ASSERT_FATAL((t3 = cupkee_timer_register(40, 0, test_handle, &v3)) != NULL);
    CU_ASSERT_FATAL((t4 = cupkee_timer_register(50, 0, test_handle, &v4)) != NULL);

    while (_cupkee_systicks < 120) {
        cupkee_timer_sync(++_cupkee_systicks);
    }

    CU_ASSERT(v1[0] == 2 && v1[1] == 2);
    CU_ASSERT(v2[0] == 2 && v2[1] == 2);
    CU_ASSERT(v3[0] == 2 && v3[1] == 2);
    CU_ASSERT(v4[0] == 2 && v4[1] == 2);

    // pass loop
    while (_cupkee_systicks < 200) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 2 && v1[1] == 2);
    CU_ASSERT(v2[0] == 2 && v2[1] == 2);
    CU_ASSERT(v3[0] == 2 && v3[1] == 2);
    CU_ASSERT(v4[0] == 2 && v4[1] == 2);


}

static void test_timer_clear(void)
{
    cupkee_timer_t *t1, *t2, *t3, *t4;

    cupkee_timer_init();
    _cupkee_systicks = 0;

    v1[0] = 0; v1[1] = 0;
    v2[0] = 0; v2[1] = 0;
    v3[0] = 0; v3[1] = 0;
    v4[0] = 0; v4[1] = 0;

    CU_ASSERT_FATAL((t1 = cupkee_timer_register(20, 1, test_handle, &v1)) != NULL);
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 1, test_handle, &v2)) != NULL);
    CU_ASSERT_FATAL((t3 = cupkee_timer_register(80, 0, test_handle, &v3)) != NULL);
    CU_ASSERT_FATAL((t4 = cupkee_timer_register(90, 0, test_handle, &v4)) != NULL);

    while (_cupkee_systicks < 35) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 1 && v1[1] == 0);
    CU_ASSERT(v2[0] == 1 && v2[1] == 0);
    CU_ASSERT(v3[0] == 0 && v3[1] == 0);
    CU_ASSERT(v4[0] == 0 && v4[1] == 0);

    // clean repeat
    CU_ASSERT(2 == cupkee_timer_clear_with_flags(1));
    while (_cupkee_systicks < 60) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 1 && v1[1] == 1);
    CU_ASSERT(v2[0] == 1 && v2[1] == 1);
    CU_ASSERT(v3[0] == 0 && v3[1] == 0);
    CU_ASSERT(v4[0] == 0 && v4[1] == 0);

    CU_ASSERT_FATAL((t1 = cupkee_timer_register(20, 1, test_handle, &v1)) != NULL);
    CU_ASSERT_FATAL((t2 = cupkee_timer_register(30, 1, test_handle, &v2)) != NULL);

    // clean no repeat
    CU_ASSERT(2 == cupkee_timer_clear_with_flags(0));
    while (_cupkee_systicks < 120) {
        cupkee_timer_sync(++_cupkee_systicks);
    }
    CU_ASSERT(v1[0] == 4 && v1[1] == 1);
    CU_ASSERT(v2[0] == 3 && v2[1] == 1);
    CU_ASSERT(v3[0] == 0 && v3[1] == 1);
    CU_ASSERT(v4[0] == 0 && v4[1] == 1);


    CU_ASSERT_FATAL((t3 = cupkee_timer_register(40, 0, test_handle, &v3)) != NULL);
    CU_ASSERT_FATAL((t4 = cupkee_timer_register(50, 0, test_handle, &v4)) != NULL);

    CU_ASSERT(1 == cupkee_timer_clear_with_id(t3->id));

    // clean all
    CU_ASSERT(3 == cupkee_timer_clear_all());

    while (_cupkee_systicks < 160) {
        cupkee_timer_sync(++_cupkee_systicks);
    }

    CU_ASSERT(v1[0] == 4 && v1[1] == 2);
    CU_ASSERT(v2[0] == 3 && v2[1] == 2);
    CU_ASSERT(v3[0] == 0 && v3[1] == 2);
    CU_ASSERT(v4[0] == 0 && v4[1] == 2);

    return;
}

CU_pSuite test_sys_timer(void)
{
    CU_pSuite suite = CU_add_suite("system timer", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "timer wakeup",    test_wakeup);
        CU_add_test(suite, "timer clear1",    test_self_clear);
        CU_add_test(suite, "timer clear2",    test_timer_clear);
    }

    return suite;
}

