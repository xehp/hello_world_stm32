

Hello STM32L4 readme.txt


This folder contains files needed to compile our "Hello World" program for the STM32L4 series.


Files and folders of interest:
CMSIS				Common Microcontroller Software Interface Standard (CMSIS)
doc				Some downloaded manuals.
STM32L4xx_HAL_Driver		Header files created by STMicroelectronics
binary.bin			The file to put onto the device (need to be built/compiled first)
Makefile			The file telling compiler which files to build.
readme.txt			This file.
system_stm32l4xx.c		Code to configure system so it will run at full speed.

The actual "hello world" source should be found in ../src because that code is also used by STM32L4 port of this program (not only STM32F3).



This program can be ported to (hopefully) any STM32F3. It was tested with Nucleo STM32F303K8T6.




So lets say we have this MCU:

STMicroelectronics
Nucleo STM32L432K8T6




To compile for this device (in addition to our own files) we needed:
1) HAL Driver header files.
2) CMSIS device files

We cloned this repository:
https://github.com/STMicroelectronics/STM32CubeL4.git
And copied over these files and folders:
	Drivers/STM32CubeL4/STM32L4xx_HAL_Driver/Inc/ 
	Drivers/CMSIS/Device/ST/STM32L4xx/Include/
	Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c
	Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/gcc/


Next we need to know what STM32L432K8T6 is called in the HAL header files.
It will be something like STM32L432x4/6/8/B/C/E, we need to know exactly which it is.
So looking in "CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h" we see this:

  /* #define STM32L412xx */   /*!< STM32L412xx Devices */
  /* #define STM32L422xx */   /*!< STM32L422xx Devices */
  /* #define STM32L431xx */   /*!< STM32L431xx Devices */
  /* #define STM32L432xx */   /*!< STM32L432xx Devices */
  /* #define STM32L433xx */   /*!< STM32L433xx Devices */
  /* #define STM32L442xx */   /*!< STM32L442xx Devices */
  /* #define STM32L443xx */   /*!< STM32L443xx Devices */
  /* #define STM32L451xx */   /*!< STM32L451xx Devices */
  /* #define STM32L452xx */   /*!< STM32L452xx Devices */
  /* #define STM32L462xx */   /*!< STM32L462xx Devices */
  /* #define STM32L471xx */   /*!< STM32L471xx Devices */
  /* #define STM32L475xx */   /*!< STM32L475xx Devices */
  /* #define STM32L476xx */   /*!< STM32L476xx Devices */
  /* #define STM32L485xx */   /*!< STM32L485xx Devices */
  /* #define STM32L486xx */   /*!< STM32L486xx Devices */
  /* #define STM32L496xx */   /*!< STM32L496xx Devices */
  /* #define STM32L4A6xx */   /*!< STM32L4A6xx Devices */
  /* #define STM32L4P5xx */   /*!< STM32L4Q5xx Devices */
  /* #define STM32L4R5xx */   /*!< STM32L4R5xx Devices */
  /* #define STM32L4R7xx */   /*!< STM32L4R7xx Devices */
  /* #define STM32L4R9xx */   /*!< STM32L4R9xx Devices */
  /* #define STM32L4S5xx */   /*!< STM32L4S5xx Devices */
  /* #define STM32L4S7xx */   /*!< STM32L4S7xx Devices */
  /* #define STM32L4S9xx */   /*!< STM32L4S9xx Devices */

There we see that STM32L432K8T6 is called STM32L432xx in the code.

NOTE If you have another device than STM32L432xx you will need to adjust the Makefile 
(see comments in Makefile on how to do that).

Did not find the needed "STM32L432xx_flash.ld" in the repository from STMicroelectronics. 
Found one over at MBED so used one from there. If you have another device than
STM32L432xx you will need to find another flash load script or create one by modifying
the "STM32L432xx_flash.ld" we have.

Install the compiler (if not installed already), like this:
	sudo apt install gcc-arm-none-eabi

Compile the code by issuing command:
	make

A "binary.bin" file should be created (among other files).

When the Nucleo is connected it shows up as a flash drive called "NODE_F303K8"
Copy the file "binary.bin" file over to that drive (into the root of the drive).

After some seconds (5 or so) the the green LED next to reset button on the Nucleo should start flashing.

If not press the reset button (it is opposite to the USB connector) and wait another 5 - 10 seconds.




