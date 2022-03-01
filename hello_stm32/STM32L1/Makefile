# Makefile
#
# This makefile is for STM32L1 series.
#
# https://github.com/xehp/hello_world_stm32
#
# Copyright (C) 2021 Alexander & Henrik Bjorkman www.eit.se/hb.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
# OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


BINARY ?= binary
ST_FLASH ?= st-flash
GCC_TOOLCHAIN ?= arm-none-eabi-

CC = $(GCC_TOOLCHAIN)gcc
AS = $(GCC_TOOLCHAIN)as

LD = $(GCC_TOOLCHAIN)ld
OBJCOPY = $(GCC_TOOLCHAIN)objcopy

# Compile all ".c" files in our own source folder.
HEADER_FILES = $(wildcard ../src/*.h)
SOURCE_FILES = $(wildcard ../src/*.c)
OBJ_FILES = $(SOURCE_FILES:.c=.o)

# Regarding names like stm32l432xx, stm32l151xB, stm32f303x8 or similar below, see readme file on how to find these names.
# TODO We can use system_stm32l1xx.c & startup_stm32l151xb.s from folder "./" or "CMSIS/Device/ST/STM32L1xx/Source/Templates".
# do not know which is better yet.
OBJ_FILES += system_stm32l1xx.o
OBJ_FILES += Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/gcc/startup_stm32l151xb.o
OBJ_FILES += ../libc/libc_init.o
OBJ_FILES += ../libc/libc_maths.o
OBJ_FILES += ../libc/libc_maths_impl.o

INCPATH = -IDrivers/STM32L1xx_HAL_Driver/Inc -IDrivers/CMSIS/Device/ST/STM32L1xx/Include -I../CMSIS/Core/Include/ -I../libc -I../src

# the "t" below  does not seem to matter, use anything (or just let it be t).
CFLAGS += -DSTM32L151xB=t

# CPU core is M3 in this case.
CFLAGS += -mcpu=cortex-m3 -mfloat-abi=soft -DHAL_FLASH_MODULE_ENABLED
ASFLAGS += -mcpu=cortex-m3 -mfloat-abi=soft
LDFLAGS += -LDrivers/CMSIS/Lib/GCC/ -l:libarm_cortexM3l_math.a

# Its not easy to find a "flash.ld" file. If something is not working it may be this file to examine.  
LDSCRIPT ?= stm32l151xb_flash.ld

# Are there some more special setting needed for floating point? Hope not.
# https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
# CFLAGS += -mfpu=fpv4-sp-d16
# ASFLAGS += -mfpu=fpv4-sp-d16

# For small projects this does not matter but its better to do it from start. One day its too late to turn these on.
CFLAGS += -Werror -Wall -Wno-pointer-sign -Wno-error=cpp

# Its a small project, if a header file is changed just recompile everything (ignoring HAL file changes, do a make clean if changing there).
DEPENDENCIES = $(HEADER_FILES) 


CFLAGS += $(INCPATH)


# This will make the committed git version available to the compiler, if there are uncommitted changes .dirty is added also.
GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"


all: info $(BINARY).hex $(BINARY).bin

%.o: %.c $(DEPENDENCIES)
	$(Q)$(CC) $(ARCH_FLAGS) $(COMMON_FLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(Q)$(AS) $(ARCH_FLAGS) $(COMMON_FLAGS) $(ASFLAGS) -o $@ $<

%.hex: %.elf
	$(Q)$(OBJCOPY) -Oihex $< $@

$(BINARY).elf: $(OBJ_FILES) $(DEPENDENCIES)
	$(Q)$(LD) $(ARCH_FLAGS) $(LDFLAGS) $(OBJ_FILES) -T $(LDSCRIPT) -o $@

$(BINARY).bin: $(BINARY).elf $(DEPENDENCIES)
	arm-none-eabi-objcopy -O binary $(BINARY).elf $(BINARY).bin

info:
	@echo "Objects:  $(OBJ_FILES)"
	@echo "Includes: $(INCPATH)"
	@echo ""

clean:
	rm $(OBJ_FILES) $(BINARY).hex $(BINARY).elf $(BINARY).bin

flash: all
	$(ST_FLASH) --format ihex write $(BINARY).hex

