

Hello STM32F3 readme.txt


This folder contains files needed to compile our "Hello World" program for the STM32F3 series.


Files and folders of interest:
CMSIS				Common Microcontroller Software Interface Standard
doc				Some downloaded manuals.
STM32F3xx_HAL_Driver		Code created by STMicroelectronics
binary.bin			The file to put onto the device (need to be built)
Makefile			The file telling compiler which files to build.
readme.txt			This file.
system_stm32f3xx.c		Code to configure system so it will run at full speed.

The actual "hello world" source should be found in ../src because that code is also used by STM32L4 port of this program (not only STM32F3).



This program can be ported to (hopefully) any STM32F3. It was tested with Nucleo STM32F303K8T6.




So lets say we have this MCU:

STMicroelectronics
Nucleo STM32F303K8T6
IC MCU 32BIT 64KB FLASH 32LQFP
ARM® Cortex®-M4 series Microcontroller IC 32-Bit 72MHz 64KB (64K x 8) FLASH 32-LQFP (7x7)

https://www.digikey.com/en/products/detail/stmicroelectronics/STM32F303K8T6/5268259?s=N4IgTCBcDaIMoBUCyBmMAxFAGFBpAHCALoC%2BQA
Series: STM32F3
Core Processor: ARM® Cortex®-M4
Core Size: 32-Bit
Speed: 72MHz
Connectivity: CANbus, I²C, IrDA, LINbus, SPI, UART/USART
Peripherals: DMA, I²S, POR, PWM, WDT
Number of I/O: 25
Program Memory Size: 64KB (64K x 8)
Program Memory Type: FLASH
EEPROM Size: -
RAM Size: 16K x 8
Voltage - Supply (Vcc/Vdd): 2V ~ 3.6V
Data Converters: A/D 9x12b; D/A 3x12b
Oscillator Type: Internal
Operating Temperature: -40°C ~ 85°C (TA)
Supplier Device Package: 32-LQFP (7x7)
Base Product Number: STM32F303




To compile for this device (in addition to the ones we already do) we need:
1) HAL Driver header files.
2) CMSIS device include

Clone this repository:
https://github.com/STMicroelectronics/STM32CubeF3.git
We do not need all files from there. These are needed:
	Drivers/STM32CubeF3/STM32F3xx_HAL_Driver/Inc/ 
	Drivers/CMSIS/Device/ST/STM32F3xx/Include/
	Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c
	Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/


Now we need to know what STM32F303K8T6 is called in the HAL header files.
It will be something like STM32F303x4/6/8 or STM32F303xB/C/E but we need to know which is is.
So looking in "STM32F3/CMSIS/Device/ST/STM32F3xx/Include/stm32f3xx.h" we see this:

	/* #define STM32F301x8 */   /*!< STM32F301K6, STM32F301K8, STM32F301C6, STM32F301C8, STM32F301R6 and STM32F301R8 Devices */
	/* #define STM32F302x8 */   /*!< STM32F302K6, STM32F302K8, STM32F302C6, STM32F302C8, STM32F302R6 and STM32F302R8 Devices */
	/* #define STM32F302xC */   /*!< STM32F302CB, STM32F302CC, STM32F302RB, STM32F302RC, STM32F302VB and STM32F302VC Devices */
	/* #define STM32F302xE */   /*!< STM32F302RE, STM32F302VE, STM32F302ZE, STM32F302RD, STM32F302VD and STM32F302ZD Devices */
	/* #define STM32F303x8 */   /*!< STM32F303K6, STM32F303K8, STM32F303C6, STM32F303C8, STM32F303R6 and STM32F303R8 Devices */
	/* #define STM32F303xC */   /*!< STM32F303CB, STM32F303CC, STM32F303RB, STM32F303RC, STM32F303VB and STM32F303VC Devices */
	/* #define STM32F303xE */   /*!< STM32F303RE, STM32F303VE, STM32F303ZE, STM32F303RD, STM32F303VD and STM32F303ZD Devices */
	/* #define STM32F373xC */   /*!< STM32F373C8, STM32F373CB, STM32F373CC, STM32F373R8, STM32F373RB, STM32F373RC, STM32F373V8, STM32F373VB and STM32F373VC Devices */
	/* #define STM32F334x8 */   /*!< STM32F334K4, STM32F334K6, STM32F334K8, STM32F334C4, STM32F334C6, STM32F334C8, STM32F334R4, STM32F334R6 and STM32F334R8 Devices */
	/* #define STM32F318xx */   /*!< STM32F318K8, STM32F318C8: STM32F301x8 with regulator off: STM32F318xx Devices */
	/* #define STM32F328xx */   /*!< STM32F328C8, STM32F328R8: STM32F334x8 with regulator off: STM32F328xx Devices */
	/* #define STM32F358xx */   /*!< STM32F358CC, STM32F358RC, STM32F358VC: STM32F303xC with regulator off: STM32F358xx Devices */
	/* #define STM32F378xx */   /*!< STM32F378CC, STM32F378RC, STM32F378VC: STM32F373xC with regulator off: STM32F378xx Devices */
	/* #define STM32F398xx */   /*!< STM32F398VE: STM32F303xE with regulator off: STM32F398xx Devices */

There we see that STM32F303K8 is simply called STM32F303x8 in the code.
If you have another device than STM32F303x8 you will need to adjust the Makefile (see comments in Makefile on how to do that).

Did not find the needed "startup_stm32f303x8.s" in the repository from STMicroelectronics. 
Did however find then in this older repository:
https://github.com/PaxInstruments/STM32CubeF3.git
It have the additional folder:
	CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/linker/
So used linker (.ld) and startup assembler (.s) files from that repository instead for now.
In those linker files find and comment out these lines:
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )

Install the compiler (if not installed already), like this:
	sudo apt install gcc-arm-none-eabi


Compile the code by issuing command:
	make


A "binary.bin" file should be created (among other files).

When the Nucleo is connected it shows up as a flash drive called "NODE_F303K8"
Copy the file "binary.bin" file over to that drive (into the root of the drive).

After some seconds (10 or so) the the green LED next to reset button on the Nucleo should start flashing.

If not press the reset button (it is opposite to the USB connector) and wait another 5 - 10 seconds.












