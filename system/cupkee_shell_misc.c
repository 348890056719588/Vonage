/*
MIT License

This file is part of cupkee project.

Copyright (c) 2016-2017 Lixing Ding <ding.lixing@gmail.com>

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

#include <cupkee.h>

#include "cupkee_shell_misc.h"

#define VARIABLE_REF_MAX    (16)
static val_t reference_vals[VARIABLE_REF_MAX];

static inline void print_number(val_t *v) {
    char buf[32];

    if (*v & 0xffff) {
        snprintf(buf, 32, "%f\r\n", val_2_double(v));
    } else {
        snprintf(buf, 32, "%lld\r\n", (int64_t)val_2_double(v));
    }
    console_puts(buf);
}

static inline void print_boolean(val_t *v) {
    console_puts(val_2_intptr(v) ? "true\r\n" : "false\r\n");
}

static inline void print_string(val_t *v) {
    console_puts("\"");
    console_puts(val_2_cstring(v));
    console_puts("\"\r\n");
}

void print_simple_value(val_t *v)
{
    if (val_is_number(v)) {
        print_number(v);
    } else
    if (val_is_boolean(v)) {
        print_boolean(v);
    } else
    if (val_is_string(v)) {
        print_string(v);
    } else
    if (val_is_undefined(v)) {
        console_puts("undefined\r\n");
    } else
    if (val_is_nan(v)) {
        console_puts("NaN\r\n");
    } else
    if (val_is_function(v)) {
        console_puts("<function>\r\n");
    } else
    if (val_is_object(v)) {
        console_puts("<object>\r\n");
    } else
    if (val_is_buffer(v)) {
        console_puts("<buffer>\r\n");
    } else
    if (val_is_array(v)) {
        console_puts("<array>\r\n");
    } else {
        console_puts("<object>\r\n");
    }
}

static void print_object_value(val_t *o)
{
    object_t *obj = (object_t *) val_2_intptr(o);
    object_iter_t it;
    const char *k;
    val_t *v;

    console_puts("{");

    _object_iter_init(&it, obj);
    if (object_iter_next(&it, &k, &v)) {
        console_puts("\r\n");
        do {
            console_puts("  ");
            console_puts(k);
            console_puts(": ");
            print_simple_value(v);
        }while(object_iter_next(&it, &k, &v));
    }
    console_puts("}\r\n");
}

static void print_array_value(val_t *v)
{
    array_t *array = (array_t *) val_2_intptr(v);
    int i, max;

    max = array_len(array);

    if (max == 0) {
        console_puts("[]\r\n");
        return;
    }
    console_puts("[\r\n");
    for (i = 0; i < max; i++) {
        char buf[16];
        snprintf(buf, 16, "  [%2d]:", i);
        console_puts(buf);

        print_simple_value(_array_elem(array, i));
    }
    console_puts("]\r\n");
}

static void print_buffer_value(val_t *v)
{
    int   i, len = _val_buffer_size(v);
    uint8_t *ptr = _val_buffer_addr(v);
    char buf[16];

    snprintf(buf, 16, "<Buffer[%d]:", len);
    console_puts(buf);
    for (i = 0; i < len && i < 8; i++) {
        snprintf(buf, 16, " %.2x", ptr[i]);
        console_puts(buf);
    }

    if (i < len) {
        console_puts(" ...>\r\n");
    } else {
        console_puts(">\r\n");
    }
}

void shell_print_value(val_t *v)
{
    if (val_is_array(v)) {
        print_array_value(v);
    } else
    if (val_is_object(v)) {
        print_object_value(v);
    } else
    if (val_is_buffer(v)) {
        print_buffer_value(v);
    } else {
        print_simple_value(v);
    }
}

void shell_reference_init(env_t *env)
{
    int i;

    for (i = 0; i < VARIABLE_REF_MAX; i++) {
        val_set_undefined(&reference_vals[i]);
    }

    env_reference_set(env, reference_vals, VARIABLE_REF_MAX);
}

val_t *shell_reference_create(val_t *v)
{
    int i;

    for (i = 0; i < VARIABLE_REF_MAX; i++) {
        val_t *r = reference_vals + i;

        if (val_is_undefined(r)) {
            *r = *v;
            return r;
        }
    }
    return NULL;
}

void shell_reference_release(val_t *ref)
{
    if (ref) {
        int pos = ref - reference_vals;

        if (pos >= 0 && pos < VARIABLE_REF_MAX) {
            val_set_undefined(ref);
        }
    }
}

void shell_print_error(int error)
{
    switch (error) {
    case ERR_NotEnoughMemory:   console_puts("Error: Not enought memory\r\n");      break;
    case ERR_NotImplemented:    console_puts("Error: Not implemented\r\n");         break;
    case ERR_StackOverflow:     console_puts("Error: Stack overflow\r\n");          break;
    case ERR_ResourceOutLimit:  console_puts("Error: Resource out of limit\r\n");   break;

    case ERR_InvalidToken:      console_puts("Error: Invalid Token\r\n");           break;
    case ERR_InvalidSyntax:     console_puts("Error: Invalid syntax\r\n");          break;
    case ERR_InvalidLeftValue:  console_puts("Error: Invalid left value\r\n");      break;
    case ERR_InvalidSementic:   console_puts("Error: Invalid Sementic\r\n");        break;

    case ERR_InvalidByteCode:   console_puts("Error: Invalid Byte code\r\n");       break;
    case ERR_InvalidInput:      console_puts("Error: Invalid input\r\n");           break;
    case ERR_InvalidCallor:     console_puts("Error: Invalid callor\r\n");          break;
    case ERR_NotDefinedId:      console_puts("Error: Not defined ID\r\n");          break;

    case ERR_SysError:          console_puts("Error: System error\r\n");            break;

    default: console_puts("Error: unknown error\r\n");
    }
}

void shell_do_callback(env_t *env, val_t *cb, uint8_t ac, val_t *av)
{
    if (!cb) return;

    if (val_is_native(cb)) {
        function_native_t fn = (function_native_t) val_2_intptr(cb);
        fn(env, ac, av);
    } else
    if (val_is_script(cb)){
        if (ac) {
            int i;
            for (i = ac - 1; i >= 0; i--)
                env_push_call_argument(env, av + i);
        }
        env_push_call_function(env, cb);
        interp_execute_call(env, ac);
    }
}

val_t shell_error(env_t *env, int code)
{
    (void) env;

    return val_mk_number(code);
}

void shell_do_callback_error(env_t *env, val_t *cb, int code)
{
    val_t err = shell_error(env, code);

    shell_do_callback(env, cb, 1, &err);
}

void shell_do_callback_buffer(env_t *env, val_t *cb, type_buffer_t *buffer)
{
    val_t args[2];

    val_set_undefined(args);
    val_set_buffer(args + 1, buffer);

    shell_do_callback(env, cb, 2, args);
}

int shell_string_id(val_t *in, int max, const char **names)
{
    if (val_is_number(in)) {
        return val_2_integer(in);
    } else {
        const char *str = val_2_cstring(in);
        if (str) {
            int id;
            for (id = 0; id < max && names[id]; id++) {
                if (!strcmp(str, names[id])) {
                    return id;
                }
            }
        }
    }
    return max;
}

val_t native_sysinfos(env_t *env, int ac, val_t *av)
{
    hw_info_t hw;
    char buf[80];

    (void) ac;
    (void) av;

    hw_info_get(&hw);

    snprintf(buf, 80, "FREQ: %uM\r\n", hw.sys_freq / 1000000);
    console_puts(buf);
    snprintf(buf, 80, "RAM: %dK\r\n", hw.ram_sz / 1024);
    console_puts(buf);
    snprintf(buf, 80, "ROM: %dK\r\n\r\n", hw.rom_sz / 1024);
    console_puts(buf);
    snprintf(buf, 80, "symbal: %d/%d, ", env->symbal_tbl_hold, env->symbal_tbl_size);
    console_puts(buf);
    snprintf(buf, 80, "string: %d/%d, ", env->exe.string_num, env->exe.string_max);
    console_puts(buf);
    snprintf(buf, 80, "number: %d/%d, ", env->exe.number_num, env->exe.number_max);
    console_puts(buf);
    snprintf(buf, 80, "function: %d/%d, ", env->exe.func_num, env->exe.func_max);
    console_puts(buf);
    snprintf(buf, 80, "variable: %d\r\n", env->main_var_num);
    console_puts(buf);

    return val_mk_undefined();
}

val_t native_systicks(env_t *env, int ac, val_t *av)
{
    (void) env;
    (void) ac;
    (void) av;

    return val_mk_number(cupkee_systicks());
}

val_t native_print(env_t *env, int ac, val_t *av)
{
    int i;

    if (ac) {
        for (i = 0; i < ac; i++) {
            shell_print_value(av+i);
        }
    } else {
        for (i = 0; i < env->native_num; i++) {
            console_puts(" * ");
            console_puts(env->native_ent[i].name);
            console_puts("\r\n");
        }
    }

    return VAL_UNDEFINED;
}

val_t native_pin_map(env_t *env, int ac, val_t *av)
{
    int id, port, pin;
    (void) env;

    if (ac < 3 || !val_is_number(av) || !val_is_number(av + 1) || !val_is_number(av + 2)) {
        return VAL_FALSE;
    }

    id   = val_2_integer(av);
    port = val_2_integer(av + 1);
    pin  = val_2_integer(av + 2);

    if (CUPKEE_OK == hw_pin_map(id, port, pin)) {
        return VAL_TRUE;
    } else {
        return VAL_FALSE;
    }
}

val_t native_led_map(env_t *env, int ac, val_t *av)
{
    int port, pin;

    (void) env;

    if (ac < 2 || !val_is_number(av) || !val_is_number(av + 1)) {
        return VAL_FALSE;
    }

    port = val_2_integer(av);
    pin  = val_2_integer(av + 1);

    if (CUPKEE_OK == hw_led_map(port, pin)) {
        return VAL_TRUE;
    } else {
        return VAL_FALSE;
    }
}

val_t native_led(env_t *env, int ac, val_t *av)
{
    (void) env;

    if (ac > 0) {
        if (val_is_true(av)) {
            hw_led_set();
        } else {
            hw_led_clear();
        }
    } else {
        hw_led_toggle();
    }
    return VAL_UNDEFINED;
}

