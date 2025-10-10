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

#ifndef __CUPKEE_NATIVE_INC__
#define __CUPKEE_NATIVE_INC__

// System
val_t native_sysinfos(env_t *env, int ac, val_t *av);
val_t native_systicks(env_t *env, int ac, val_t *av);
val_t native_show(env_t *env, int ac, val_t *av);

// User scripts
val_t native_scripts(env_t *env, int ac, val_t *av);

// Timer
val_t native_set_timeout(env_t *env, int ac, val_t *av);
val_t native_set_interval(env_t *env, int ac, val_t *av);
val_t native_clear_timeout(env_t *env, int ac, val_t *av);
val_t native_clear_interval(env_t *env, int ac, val_t *av);

/*
 * Device api
 */
val_t device_native_create(env_t *env, int ac, val_t *av);
val_t device_native_pin_map(env_t *env, int ac, val_t *av);
val_t device_native_led_map(env_t *env, int ac, val_t *av);
val_t device_native_led(env_t *env, int ac, val_t *av);

// Filter api
val_t filter_native_crate(env_t env, int ac, val_t *av);


#endif /* __CUPKEE_NATIVE_INC__ */

