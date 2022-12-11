/*
Barebone blinky on nucleo L432KC / stm32l432kc using SysTick. No RTOS needed.

https://github.com/xehp/hello_world_stm32

Copyright (C) 2021 Alexander & Henrik Bjorkman www.eit.se/hb.

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

This module holds the SysLib. Some functions to administrate the system itself.
Provides milli seconds tick timer, sleep and error handler.
It will use one IO pin to show status and error.
*/

#ifndef SYSTEMINIT_H
#define SYSTEMINIT_H

#if (defined STM32L432xx)
#include "stm32l4xx.h"
#include "stm32l4xx_hal_cortex.h"
#elif (defined STM32L151xB)
#include "stm32l1xx.h"
#include "stm32l1xx_hal_cortex.h"
#elif (defined STM32F303x8)
#include "stm32f3xx.h"
#include "stm32f3xx_hal_cortex.h"
#elif (defined STM32F030x8)
#include "stm32f0xx.h"
#include "stm32f0xx_hal_cortex.h"
#elif (defined STM32F103xB)
#include "stm32f1xx.h"
#include "stm32f1xx_hal_cortex.h"
#else
#error Unknown target CPU/MCU
#endif

#include <inttypes.h>
#include "version.h"
#include "dev_cfg.h"

// Legacy
#define SysClockFrequencyHz SystemCoreClock


enum
{
	SysMorseJ  = -14, //  .---
	SysMorseP  = -13, // .--.
	SysMorse12 = -12, //  .-.-
	SysMorseL  = -11, // .-..
	SysMorse10 = -10, //  ..--
	SysMorseF  = -9,  // ..-.
	SysMorseV  = -8,  // ...-
	SysMorseH  = -7,  // ....
	SysMorseW  = -6,  // .--
	SysMorseR  = -5,  // .-.
	SysMorseU  = -4,  // ..-
	SysMorseS  = -3,  // ...
	SysMorseA  = -2,  // .-
	SysMorseI  = -1,  // ..
	SysMorseE  = 0,   // .
	SysMorseT  = 1,   // -
	SysMorseN  = 2,   // -.
	SysMorseM  = 3,   // --
	SysMorseD  = 4,   // -..
	SysMorseK  = 5,   // -.-
	SysMorseG  = 6,   // --.
	SysMorseO  = 7,   // ---
	SysMorseB  = 8,   // -...
	SysMorseX  = 9,   // -..-
	SysMorseC  = 10,  // -.-.
	SysMorseY  = 11,  // -.--
	SysMorseZ  = 12,  // --..
	SysMorseQ  = 13,  // --.-

} /*SysMorseCodes*/;


// More enums can be added here if needed.
enum
{                                     // Morse code
	SYS_ERROR_FROM_MX_MAIN = 2,       // N -.
	SYS_ASSERT_ERROR = 3,             // M --
	SYS_USART_ERROR = 4,              // D -..
	SYS_FLASH_DRIVER_ERROR = 7,       // O ---
	STS_ADC_DRIVER_ERROR = 8,         // B -...
	SYS_APPLICATION_ASSERT_ERROR = 9, // X -..-
	SYS_SOFT_UART_ERROR = 10,         // C -.-.
	SYS_UNKNOWN_UART_ERROR = 11,      // Y -.--
	//SYS_CAN_ERROR = 12,             // Z --..
} /*SysErrorCodes*/;



/**
 * These are part of system library since the system library error handler
 * needs them also. It will use this to operate the green debug LED.
 * These functions can also be used to operate other output pins.
 *
 * On nucleo l432kc the Green LED is on port B pin 3
 * If changing port remember to also activate the clock for that port.
 * For port B that is the line: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
 * For port C that is the line: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
 * It will be initiated by system library to be used by error handler if needed.
 * This also needs to be changed if the application is to use this pin 
 * for something else.
 */
#ifdef DEBUG_LED_ACTIVE_LOW
#define SYS_LED_PORT DEBUG_LED_PORT
#define SYS_LED_PIN DEBUG_LED_PIN
#define SYS_LED_OFF() SYS_LED_PORT->ODR |= (0x1U << SYS_LED_PIN);
#define SYS_LED_ON() SYS_LED_PORT->ODR &= ~(0x1U << SYS_LED_PIN);
#elif (defined DEBUG_LED_ACTIVE_HIGH)
#define SYS_LED_PORT DEBUG_LED_PORT
#define SYS_LED_PIN DEBUG_LED_PIN
#define SYS_LED_ON() SYS_LED_PORT->ODR |= (0x1U << SYS_LED_PIN);
#define SYS_LED_OFF() SYS_LED_PORT->ODR &= ~(0x1U << SYS_LED_PIN);
#else
// No system LED available
#define SYS_LED_ON()
#define SYS_LED_OFF()
#endif

void sysPinOutInit(GPIO_TypeDef* port, int pin);
void sysPinOutSetHigh(GPIO_TypeDef* port, int pin); // For Green system LED this is On
void sysPinOutSetLow(GPIO_TypeDef* port, int pin); // For Green system LED this is Off
void sysLedFlashCode(int code);

/**
 * Application functions can call this to get the system tick counter.
 * It is supposed to be one tick per milli second.
 */
int32_t sysGetSysTimeMs();

/**
 * For very short delays the systemBusyWait can be used.
 * However the delay length of systemBusyWait is unspecified.
 * 
 * For longer delays it is much better to use the systemSleepMs. 
 * 
 * It is recommended that the main loop or other long
 * lasting loops calls systemSleep() once per turn.
 */
void sysBusyWait(uint32_t delay_uS);
void sysSleepMs(int32_t time_to_sleep_mS);
void sysSleep();
void sysWdtReset();

/**
 * Device handlers or applications can call this if they encounter an
 * error after which system shall be halted. Typically SysLib will
 * go into an eternal loop flashing the green LED depending on error code.
 * See enum SystemErrorCodes for error codes to use.
 */
void sysErrorHandler(int errorCode);

// This shall be first function called from main.
// This is not same as systemInit which is called just before main is called.
void sysInit(void);


#define SYS_ASSERT(c) {if (!(c)) {sysErrorHandler(SYS_ASSERT_ERROR);}}


#define sys_disable_interrupts() __disable_irq()
#define sys_enable_interrupts() __enable_irq()



// Check configuration

#if (VERSION_PCB <= 3) && (!defined STM32F030x8)
#error
#endif

#if (VERSION_PCB >= 4) && (VERSION_PCB <=8) && (!defined STM32F103xB)
#error
#endif

#endif
