

Hello STM32WB readme.txt (adapted from STM32F3)


This folder contains files needed to compile our "Hello World" program for the STM32WB series.
Or rather instead of having all files here we explain where to get them and where to put them.
Because that is more future prof and its more instructive.


Files and folders of interest (some need to be created/copied):
Drivers				Common Microcontroller Software Interface Standard and HAL macro files.
doc				Some downloaded manuals (optional).
binary.bin			The file to put onto the device (need to be built)
Makefile			The file telling compiler which files to build.
readme.txt			This file.
system_stm32f3xx.c	Optional file with code to configure system so it will run at full speed.

The actual "hello world" source should be found in ../src because that code is also used by STM32L4 port of this program (not only STM32WB).



This program can be ported to (hopefully) any STM32WB. It was tested with Nucleo STM32WB35CCU6A (not yet tested).




So lets say we have this MCU: STM32WB35CCU6A

ULTRA-LOW-POWER DUAL CORE ARM CO
IC RF TxRx + MCU 802.15.4, Bluetooth Bluetooth v5,0, Thread, Zigbee® 2,405GHz ~ 2,48GHz 
48-UFQFN frilagt öra
256kB flash, 192kB SRAM


To compile for this device (in addition to the ones we already have) we need:
1) HAL Driver header files.
2) CMSIS device include

Clone this repository:
https://github.com/STMicroelectronics/STM32CubeWB
The easy way is to simply copy the folder "Drivers" from there to here.
Although we do not need all files from there. These files/folders are needed:
        STM32WBxx_HAL_Driver/Inc 
	CMSIS/Device/ST/STM32WBxx/Include
	CMSIS/Device/ST/STM32WBxx/Source/Templates/system_stm32f3xx.c
	CMSIS/Device/ST/STM32WBxx/Source/Templates/gcc
If the desired structure is unclear compare with respective folder for STM32L4 (for illustration we have copy all needed files for STM32L4 already).


Now we need to know what STM32WB35CCU6A is called in the HAL header files.
So looking in "STM32F3/CMSIS/Device/ST/STM32WBxx/Include/stm32wbxx.h".
There we see that STM32WB35CCU6A is simply called STM32WB35xx in the code.


If you have another device than STM32WB35CCU6A you will need to adjust the Makefile (see comments in Makefile on how to do that).


In folder CMSIS/Device/ST/STM32WBxx/Source/Templates/gcc
Have a look in startup_stm32wb35xx_cm4.s & linker/stm32wb35xx_flash_cm4.ld

For other STM32s I had to find and comment out these lines:
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
(Seems I did not need to do so in this case.)


Install the compiler (if not installed already), like this:
	sudo apt install gcc-arm-none-eabi


Compile the code by issuing command:
	make


If you get "No rule to make target" or "cannot open linker script file" then check instructions above again,
probably files and folders did not go into the folders exactly as needed.
If the error is about "libc.a" then be sure to comment those lines out in the linker file.


A "hello_stm.hex" file should be created (among other files).


Other HW (non Nucleo etc) using external ST-LINK V2 dongle.

https://freeelectron.ro/installing-st-link-v2-to-flash-stm32-targets-on-linux/
https://github.com/tomvdb/stm32l1-discovery-basic-template
If you are not able to communicate with the STM32 board without root privileges you should follow the step from the stlink repo readme file for adding a udev rule for this hardware.
https://github.com/stlink-org/stlink/commit/6471a60460a4659134cf80d8864a022dc09b8447


st-flash --connect-under-reset --format ihex write binary.hex

If it does not to download code work check BOOT0 and that the st-link SW is at least 1.7.

If device does not boot after flashing check the BOOTx pins.



When working I sometimes forget to make, so use this to write:
make && st-flash --connect-under-reset --format ihex write hello_stm.hex




After some seconds the the green system/debug LED should start flashing.




When testing this SW the LED did flash as wanted. Did not manage to get serial port working (could be HW issue also).







