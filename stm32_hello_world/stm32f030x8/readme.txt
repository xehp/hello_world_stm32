


Install STM32CubeMX

file:///home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/Readme.html
/home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/SetupSTM32CubeMX-6.4.0
/home/henrik/STM32CubeMX/STM32CubeMX



Start my project from MCU

Have this one: STM32F030C8T6
Last two chars are only temp range.
So select: STM32F030C8


Pinout & Configuration

Enable IWDG
Set clock prescaler to max (256)


Under Pinout and configuration set PF0/1 to RCC_OSC_IN/OUT


Clock configuration
Input frequency 12 MHz
PLL Source: HSE 
PLLMul: x 3
System Clock Mux: PLLCLK
This will give HCLK 36 MHz


Project manager
Enter a name "stm32f030x8_hse"
and location "/home/henrik/git/lox/Firmware/RCLD_RoadTrailerLock"
Set Toolchain / IDE -> Makefile

Press "Generate code"


Add following user code in main.c
-------------------------------------------
/* USER CODE BEGIN Includes */
#include "main_loop.h"
-------------------------------------------
  /* USER CODE BEGIN WHILE */
  main_loop();
-------------------------------------------
  /* USER CODE BEGIN Error_Handler_Debug */
  sysErrorHandler(SYS_ERROR_FROM_MX_MAIN);
-------------------------------------------


In Makefile add this
-------------------------------------------
# Compile all ".c" files in our own source folder.
C_SOURCES += $(wildcard ../src/*.c)
-------------------------------------------
C_INCLUDES += -I../inc
-------------------------------------------
# This will make the commited git version available to the compiler, if there are uncommited changes .dirty is added also.
GIT_VERSION := $(shell git describe --abbrev=8 --dirty)
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
-------------------------------------------




make && st-flash --format ihex write build/stm32f030x8.hex


