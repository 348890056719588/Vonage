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

#ifndef __CUPKEE_INC__
#define __CUPKEE_INC__

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


/* Todo: put to cupkee_type.h ? */
#define CUPKEE_TRUE                 1
#define CUPKEE_FALSE                0

#define CUPKEE_MEMBER_OFFSET(T, m) (intptr_t)(&(((T *)0)->m))
#define CUPKEE_CONTAINER_OF(p, T, m) ((T*)((intptr_t)(p) - MEMBER_OFFSET(T, m)))

/* User configure ? */
#define APP_DEV_MAX                 8

#include "cupkee_bsp.h"
#include "cupkee_errno.h"
#include "cupkee_utils.h"
#include "cupkee_memory.h"
#include "cupkee_event.h"
#include "cupkee_buffer.h"
#include "cupkee_device.h"
#include "cupkee_console.h"
#include "cupkee_auto_complete.h"
#include "cupkee_history.h"
#include "cupkee_command.h"
#include "cupkee_shell.h"
#include "cupkee_native.h"

void cupkee_init(void);
void cupkee_loop(void);

int  cupkee_event_handle_register(cupkee_event_handle_t handle);
uint32_t cupkee_systicks(void);

#endif /* __CUPKEE_INC__ */

