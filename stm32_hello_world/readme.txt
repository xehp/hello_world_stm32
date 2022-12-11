
This is a hello world program for STM32





A cross compiler will be needed. If not already installed just do:
sudo apt install gcc-arm-none-eabi stlink-tools



cd stm32f030x8

make && st-flash --format ihex write build/stm32f030x8.hex


If all goes well it shall say: "Flash written and verified!".





Copyright (C) 2022 Henrik Bjorkman www.eit.se/hb.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.





Porting to other STM32 chip families using CubeMX




https://www.st.com/en/development-tools/stm32cubemx.html
cd downloaded_stm32_cube_mx_installation_files
sudo ./SetupSTM32CubeMX-6.6.1

Or try one of ther this java did not work:
	sudo apt install default-jre
	sudo apt-get install openjdk-11-jdk


Installed in /usr/local/STMicroelectronics/STM32Cube/STM32CubeMX
Also got a file "auto-install.xml" that supposedly will make it easier to install it on more computers.

Next start:
/usr/local/STMicroelectronics/STM32Cube/STM32CubeMX/STM32CubeMX &

Will try adding support for STM32F103C8T6 below.
(Last two chars are only temp range so sometimes its called just STM32F103C8 or STM32F103xB).

Select MCU and an image with the CPU pins shall show up.
Will do all serial port stuff in own code, so just clocks and watchdog to setup here.
Later we may need CAN or USB settings (note those two use same buffer so can't be used simultaniously).


System Core

	Enable IWDG
	Set clock prescaler to max (256), that give us 30+ seconds before watchdog timeouts.

Connectivity
	Enable USART1, USART2, USART3 (if available)
	Check that baudrate is 115200.


Middleware
	USB_DEVICE make its class for FS IP: "Communication Device Class (Virtual Port Com)"
	Read more at:
	https://controllerstech.com/send-and-receive-data-to-pc-without-uart-stm32-usb-com/

Clock configuration
	If using internal RC then no changes are needed here. 
	We have an external 12 MHz chrystal so want to use that.
		In pinout view assign PD0/1 to RCC_OSC_IN/OUT
		Under System Core -> RCC -> HSE set it to Crystal/Ceramic resonator
		In Clock Configuration its now possible the select HSE, set System clock Mux to PLLCLK and HCLK to 72 MHz (or 48 MHz).


Project manager
	Enter a name and location: stm32f103xB, /home/henrik/git/stm32_booter/stm32_hello_world/
	Set Makefile as Toolchain / IDE

Press "Generate code"
Allow downloading missing code.


The generated file "stm32f103x8.ioc" can be used if project need to be adjusted.


In file "...Core/Src/main.c"
find line "/* USER CODE BEGIN Includes */" and add:
#include "main_loop.h"


find line "/* USER CODE BEGIN 2 */" and add (for uarts available):
  HAL_UART_Transmit(&huart1, (uint8_t*)"huart1\n", 7, 1000000);
  HAL_UART_Transmit(&huart2, (uint8_t*)"huart2\n", 7, 1000000);
  HAL_UART_Transmit(&huart2, (uint8_t*)"huart2\n", 7, 1000000);


find line "/* USER CODE BEGIN WHILE */" and add:
main_loop();

In file "Makefile"
Find "C_INCLUDES" and after that add:
C_INCLUDES += -I../inc
Find "C_SOURCES" and after that add:
C_SOURCES += $(wildcard ../src/*.c)






Install GCC for arm
sudo apt install gcc-arm-none-eabi

cd to relevant folder.
make


Install stlink and flash the chip to see if it runns.
st-flash --connect-under-reset --format ihex write build/hello_cube.hex



