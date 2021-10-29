# Makefile
#
# This makefile is for STM32F3 series.
# The orriginal divice this was written for was STM32F303K8T6.
# To use another STM32F3 MCU find out its name in the macro files and 
# replace all stm32f303x8 with that other device. 
# See the readme file for information on how to figure out the macro name.
# 



BINARY_NAME ?= binary
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

OBJ_FILES += CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303x8.o
#OBJ_FILES += CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.o
OBJ_FILES += system_stm32f3xx.o
OBJ_FILES += ../libc/libc_init.o
OBJ_FILES += ../libc/libc_maths.o
OBJ_FILES += ../libc/libc_maths_impl.o



INCPATH = -ISTM32F3xx_HAL_Driver/Inc -ICMSIS/Device/ST/STM32F3xx/Include/ -I../CMSIS/Core/Include/ -I../libc -I../src


# the "t" below  does not seem to matter, use anything (or just let it be t).
CFLAGS += -DSTM32F303x8=t

# CPU core is M4 in this case.
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mcpu=cortex-m4 -DHAL_FLASH_MODULE_ENABLED
ASFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=soft

LDFLAGS += -L../CMSIS/Lib/GCC -l:libarm_cortexM4lf_math.a

# Very unsure about which libgcc.a to use (there are many)
LDFLAGS += -L/usr/lib/arm-none-eabi/lib -L/usr/lib/gcc/arm-none-eabi/9.2.1

LDSCRIPT ?= CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/linker/STM32F303X8_FLASH.ld

CFLAGS += -Werror -Wall -Wno-pointer-sign


# Its a small project, if a header file is changed just recompile everything (ignoring HAL file changes, do a make clean if changing there).
DEPENDENCIES = $(HEADER_FILES) 




# This will make the committed git version available to the compiler, if there are uncommitted changes .dirty is added also.
GIT_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)
CFLAGS += $(INCPATH) -DGIT_VERSION=\"$(GIT_VERSION)\"


all: info $(BINARY_NAME).hex $(BINARY_NAME).bin

%.o: %.c $(DEPENDENCIES)
	$(Q)$(CC) $(ARCH_FLAGS) $(COMMON_FLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(Q)$(AS) $(ARCH_FLAGS) $(COMMON_FLAGS) $(ASFLAGS) -o $@ $<

%.hex: %.elf
	$(Q)$(OBJCOPY) -Oihex $< $@

$(BINARY_NAME).elf: $(OBJ_FILES) $(DEPENDENCIES)
	$(Q)$(LD) $(ARCH_FLAGS) $(LDFLAGS) $(OBJ_FILES) -T $(LDSCRIPT) -o $@

$(BINARY_NAME).bin: $(BINARY_NAME).elf $(OBJ_FILES) $(DEPENDENCIES)
	arm-none-eabi-objcopy -O binary $(BINARY_NAME).elf $(BINARY_NAME).bin

info:
	@echo "Objects:  $(OBJ_FILES)"
	@echo "Includes: $(INCPATH)"
	@echo ""

clean:
	rm $(OBJ_FILES) $(BINARY_NAME).hex $(BINARY_NAME).elf $(BINARY_NAME).bin

flash: all
	$(ST_FLASH) --format ihex write $(BINARY_NAME).hex

