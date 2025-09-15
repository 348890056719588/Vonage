/*
MIT License

This file is part of cupkee project.

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

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


#include "test.h"

static const native_t native_entry[] = {
    /* cupkee native */
    {"Buffer",          buffer_native_create},
    {"sysinfos",        native_sysinfos},
    {"systicks",        native_systicks},

    {"show",            native_show},

    {"setTimeout",      native_set_timeout},
    {"setInterval",     native_set_interval},
    {"clearTimeout",    native_clear_timeout},
    {"clearInterval",   native_clear_interval},

    {"scripts",         native_scripts},

    {"Device",          device_native_create},
    /* user native */
};

int test_cupkee_reset(void)
{
    hw_dbg_reset();

    cupkee_init();
    cupkee_set_native(native_entry, sizeof(native_entry)/sizeof(native_t));

    return 0;
}

int test_cupkee_start(const char *init)
{
    cupkee_start(init);

    return test_cupkee_run_with_reply("\r", NULL, 1);
}

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /***********************************************
     * Test suites here:
     ***********************************************/
    test_hello();

    test_util_buffer();

    test_system_misc();
    test_device_pin();
    test_device_adc();
    test_device_pwm();
    test_device_uart();

    // add test suite

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

