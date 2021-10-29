# Makefile
#
# This makefile is for STM32L4 series.
#
# Use this if less output is wanted during compiling.
#ifndef V
#	Q := @
#endif



BINARY ?= binary
ST_FLASH ?= st-flash
GCC_TOOLCHAIN ?= arm-none-eabi-

CC = $(GCC_TOOLCHAIN)gcc
AS = $(GCC_TOOLCHAIN)as

LD = $(GCC_TOOLCHAIN)ld
OBJCOPY = $(GCC_TOOLCHAIN)objcopy

# Compile all "c" files in our source folder.
HEADER_FILES = $(wildcard ../src/*.h)
SOURCE_FILES = $(wildcard ../src/*.c)
OBJ_FILES = $(SOURCE_FILES:.c=.o)

# Regarding names like stm32l432xx, stm32l151xB, stm32f303x8 or similar below, see comment for "which macro to use" further down.
OBJ_FILES += startup_stm32l432xx.o
OBJ_FILES += system_stm32l4xx.o
OBJ_FILES += ../libc/libc_init.o
OBJ_FILES += ../libc/libc_maths.o
OBJ_FILES += ../libc/libc_maths_impl.o

INCPATH = -ISTM32L4xx_HAL_Driver/Inc -I../CMSIS/Device/ST/STM32L4xx/Include/ -I../CMSIS/Core/Include/ -I../libc -I../src -I./ -ISTM32L4xx_HAL_Driver


# the "t" below  does not seem to matter, use anything (or just let it be t).
CFLAGS += -DSTM32L432xx=t

# CPU core is M4 in this case.
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mcpu=cortex-m4 -DHAL_FLASH_MODULE_ENABLED
ASFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=soft
LDFLAGS += -L../CMSIS/Lib/GCC -l:libarm_cortexM4l_math.a
LDSCRIPT ?= STM32L432xx_flash.ld


CFLAGS += -Werror -Wall -Wno-pointer-sign


# Its a small project, if a header file is changed just recompile everything (ignoring HAL file changes, do a make clean if changing there).
DEPENDENCIES = $(HEADER_FILES) 




# This will make the committed git version available to the compiler, if there are uncommitted changes .dirty is added also.
GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)
CFLAGS += $(INCPATH) -DGIT_VERSION=\"$(GIT_VERSION)\"


all: info $(BINARY).hex $(BINARY).bin

%.o: %.c $(DEPENDENCIES)
	$(Q)$(CC) $(ARCH_FLAGS) $(COMMON_FLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(Q)$(AS) $(ARCH_FLAGS) $(COMMON_FLAGS) $(ASFLAGS) -o $@ $<

%.hex: %.elf
	$(Q)$(OBJCOPY) -Oihex $< $@

$(BINARY).elf: $(OBJ_FILES) $(DEPENDENCIES)
	$(Q)$(LD) $(ARCH_FLAGS) $(LDFLAGS) $(OBJ_FILES) -T $(LDSCRIPT) -o $@

$(BINARY).bin: $(BINARY).elf
	arm-none-eabi-objcopy -O binary $(BINARY).elf $(BINARY).bin

info:
	@echo "Objects:  $(OBJ_FILES)"
	@echo "Includes: $(INCPATH)"
	@echo ""

clean:
	rm $(OBJ_FILES) $(BINARY).hex $(BINARY).elf $(BINARY).bin

flash: all
	$(ST_FLASH) --format ihex write $(BINARY).hex

