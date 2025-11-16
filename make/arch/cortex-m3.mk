##
## MIT License
##
## This file is part of cupkee project.
##
## Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
##



LDSCRIPT    = ${MAKE_DIR}/ld/$(MCU).ld

ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd

OPENCM3_DIR = ${BASE_DIR}/module/libopencm3
OPENCM3_LIB = opencm3_stm32f1

DEFS		+= -DSTM32F1
DEFS		+= -I$(OPENCM3_DIR)/include

ARCH_LDFLAGS += -T$(LDSCRIPT) -L$(OPENCM3_DIR)/lib -l$(OPENCM3_LIB)
ARCH_LDFLAGS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
ARCH_LDFLAGS += --static -nostartfiles
ARCH_LDFLAGS += -Wl,--gc-sections
ifeq ($(V),99)
ARCH_LDFLAGS += -Wl,--print-gc-sections
endif

PREFIX  ?= arm-none-eabi-

