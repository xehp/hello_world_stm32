
So lets say we have this MCU:
	STMicroelectronics
	STM32L151C8T6
	IC MCU 32BIT 64KB FLASH 48LQFP
	ARM® Cortex®-M3 STM32L1 Mikrokontroller IC 32-bitar 32MHz 64KB (64K x 8) FLASH 48-LQFP (7x7)

	https://www.digikey.se/product-detail/sv/stmicroelectronics/STM32L151C8T6/497-11190-ND/2640836
		Series: STM32L1
		Core Processor: ARM® Cortex®-M3
		Core Size: 32-Bit
		Speed: 32MHz
		Program Memory Size: 64KB (64K x 8)
		EEPROM-size: 4K x 8 
		RAM Size: 10K x 8
		Voltage - Supply (Vcc/Vdd): 1.8V ~ 3.6V
		Oscillator Type: Internal
		Supplier Device Package: 48-LQFP (7x7)





To compile for this device (in addition to the ones we already do) we need:
1) HAL Driver header files.
2) CMSIS device include

Clone this repository:
https://github.com/STMicroelectronics/STM32CubeL1.git
We do not need all files from there. These are needed:
	Drivers/STM32CubeL1/STM32L1xx_HAL_Driver/Inc/ 
	Drivers/CMSIS/Device/ST/STM32L1xx/Include/
	Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/system_stm32l1xx.c
	Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/gcc/


Now we need to know what STM32L151C8T6 is called in the HAL header files.
It will be something like STM32L151x8 or STM32L151xB etc but we need to know which is is.
So in CMSIS we have "Drivers/CMSIS/Device/ST/STM32L1xx/Include/stm32l1xx.h" and there 
we see that the macro name to use for STM32L151C8 is STM32L151xB.



Did not find the needed "startup_stm32l151xB.s" in the repository above. Did find then in this older repository:
https://github.com/PaxInstruments/STM32CubeF1.git
It have the additional folder:
	CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/linker/
So used linker (.ld) and startup assembler (.s) files from that repository instead for now.
In those linker files find and comment out these lines:
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )



Next modify the Makefile so it uses needed files (from the files and folders mentioned above).


make and test it.


When a Nucleo is connected it shows up as a flash drive called "STM32L151C8"
Copy the build binary.bin file over to root in "STM32L151C8".

When it works the green LED next to reset button on the Nucleo should start flashing after a while.

If not, NOTE that in our testing on STM32L1 we did not use a Nucleo, instead it was a proprietary board,
see doc/schematics.pdf. The debug LED was on pin PC13 (not PB3 as typical for Nucleos). 
This will then need to be changed in file cfg.h



Other HW (non Nucleo etc) using external ST-LINK V2 dongle.

https://freeelectron.ro/installing-st-link-v2-to-flash-stm32-targets-on-linux/
https://github.com/tomvdb/stm32l1-discovery-basic-template
If you are not able to communicate with the STM32 board without root privileges you should follow the step from the stlink repo readme file for adding a udev rule for this hardware.
https://github.com/stlink-org/stlink/commit/6471a60460a4659134cf80d8864a022dc09b8447


st-flash --connect-under-reset --format ihex write binary.hex

If it does not to download code work check BOOT0 and that the st-link SW is at least 1.7.

If device does not boot after flashing check the BOOTx pins.




