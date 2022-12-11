
This is a demo program for stm32f030x8

Install needed programs

A JRE and a cross compiler will be needed. If not already installed just do:
sudo apt install default-jre gcc-arm-none-eabi stlink-tools



Install STM32CubeMX

file:///home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/Readme.html
/home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/SetupSTM32CubeMX-6.4.0
/home/henrik/STM32CubeMX/STM32CubeMX

cd downloaded_stm32_cube_mx_installation_files
sudo ./SetupSTM32CubeMX-6.6.1


Installed in /usr/local/STMicroelectronics/STM32Cube/STM32CubeMX
Also got a file "auto-install.xml" that supposedly will make it easier to install it on more computers.

Next start:
/usr/local/STMicroelectronics/STM32Cube/STM32CubeMX/STM32CubeMX &



Choose "Start my project from MCU"

Find the wanted MCU (last two chars are only temp range).
	See stm32f030x8/readme.txt for example.

Pinout & Configuration




Will try adding support for STM32F103C8T6 below.
(Last two chars are only temp range so sometimes its called just STM32F103C8 or STM32F103xB).

Select MCU and an image with the CPU pins shall show up.
Will do all serial port stuff in own code, so just clocks and watchdog to setup here.
Later we may need CAN or USB settings (note those two use same buffer so can't be used simultaniously).


Select PB3 and set it as GPIO_Output

System Core
	

Clock configuration
	If using internal RC then no changes are needed here. 
	See stm32f030x8/readme.txt if external crystal is wanted.

Project manager
	Enter a name "stm32f030x8"
	and location "/home/henrik/git/lox/Firmware/RCLD_RoadTrailerLock"
	Set Toolchain / IDE -> Makefile

Press "Generate code"
Allow downloading missing code.


The generated file "stm32f103x8.ioc" can be used if project need to be adjusted.



In main.c find lines:
-------------------------------------------
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
-------------------------------------------
Add some code there:
-------------------------------------------
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    volatile int i = 0;
    for(i=0;i<0x800000;++i) {;}
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    for(i=0;i<0x80000;++i) {;}
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

    for(i=0;i<0x80000;++i) {;}
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    for(i=0;i<0x80000;++i) {;}
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);


    /* USER CODE END WHILE */
-------------------------------------------


The above delay of 0x800000 resp 0x80000 shall be OK at 40-80 MHz.
If running at 8 MHz then try 0x100000 resp 0x10000

NOTE On nucleo-f103rb the LED is on GPIOA pin 5.



cd to relevant folder such as "stm32f030x8" or "stm32f103xB".
make


Install stlink and flash the chip to see if it runs.


make && st-flash --format ihex write build/stm32f103xB.hex


If all goes well it shall say: "Flash written and verified!".



When running the LED at PB3 should flash. 
Have patiance it may flash very slowly if clock is at 8MHz only.




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
