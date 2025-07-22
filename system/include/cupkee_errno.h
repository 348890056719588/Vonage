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

#ifndef __CUPKEE_ERRNO_INC__
#define __CUPKEE_ERRNO_INC__

/************************************************************************
 * Cupkee error code
 ***********************************************************************/
#define CUPKEE_OK               0       // not implement
#define CUPKEE_ERROR            10000   // error not in list:
#define CUPKEE_EIMPLEMENT       20000   // not implement
#define CUPKEE_ENAME            20001   // invalid device name
#define CUPKEE_EINVAL           20002   // invalid argument
#define CUPKEE_ERESOURCE        20003   // not enought resource
#define CUPKEE_EENABLED         20004   // config set for device that already enabled
#define CUPKEE_ENOTENABLED      20005   // write & read device that not enabled
#define CUPKEE_EHARDWARE        21000   // hardware error
#define CUPKEE_ESETTINGS        21001   // invalid settings

#endif /* __CUPKEE_ERRNO_INC__ */

