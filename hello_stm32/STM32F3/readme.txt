
So lets say we have this MCU:

STMicroelectronics
STM32F303K8T6
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
So in CMSIS we have "stm32f3xx.h" and there we see that STM32F303K8 is simply STM32F303x8.


Did not find the needed "startup_stm32f303x8.s" in the repository above. Did find then in this older repository:
https://github.com/PaxInstruments/STM32CubeF3.git
It have the additional folder:
	CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/linker/
So used linker (.ld) and startup assembler (.s) files from that repository instead for now.
In those linker files find and comment out these lines:
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )



Next modify the Makefile so it uses needed files (from the files and folders mentioned above).


make and test it.

When the Nucleo is connected it shows up as a flash drive called "NODE_F303K8"
Copy the build binary.bin file over to root in "NODE_F303K8".

When it works the green LED next to reset button on the Nucleo should start flashing after a while.




TODO Make our own system_stm32f3xx.c file so that we can use other system clock frequencies than default.
