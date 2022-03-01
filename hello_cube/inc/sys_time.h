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
#else
#error Unknown target CPU/MCU
#endif

#include <inttypes.h>
#include "cfg.h"




// More enums can be added here if needed.
enum
{                                     // Morse code
	SYS_ERROR_FROM_MX_MAIN = 2,       // N -.
	SYS_ASSERT_ERROR = 3,             // M --
	SYS_USART1_ERROR = 4,             // D -..
	SYS_USART2_ERROR = 5,             // K -.-
	SYS_LPUART_ERROR = 6,             // G --.
	SYS_FLASH_DRIVER_ERROR = 7,       // O ---
	STS_ADC_DRIVER_ERROR = 8,         // B -...
	SYS_APPLICATION_ASSERT_ERROR = 9, // X -..-
	SYS_SOFT_UART_ERROR = 10,         // C -.-.
	SYS_UNKNOWN_UART_ERROR = 11,      // Y -.--
} SysErrorCodes;



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
#define SYS_LED_PORT DEBUG_LED_PORT
#define SYS_LED_PIN DEBUG_LED_PIN

#ifdef DEBUG_LED_ACTIVE_LOW
#define SYS_LED_OFF() SYS_LED_PORT->ODR |= (0x1U << SYS_LED_PIN);
#define SYS_LED_ON() SYS_LED_PORT->ODR &= ~(0x1U << SYS_LED_PIN);
#elif (defined DEBUG_LED_ACTIVE_HIGH)
#define SYS_LED_ON() SYS_LED_PORT->ODR |= (0x1U << SYS_LED_PIN);
#define SYS_LED_OFF() SYS_LED_PORT->ODR &= ~(0x1U << SYS_LED_PIN);
#else
#warning No system LED available
#endif

void sysPinOutInit(GPIO_TypeDef* port, int pin);
void sysPinOutSetHigh(GPIO_TypeDef* port, int pin); // For Green system LED this is On
void sysPinOutSetLow(GPIO_TypeDef* port, int pin); // For Green system LED this is Off

/**
 * Application functions can call this to get the system tick counter.
 * It is supposed to be one tick per milli second.
 */
int64_t sysGetSysTimeMs();

/**
 * For very short delays the systemBusyWait can be used.
 * However the delay length of systemBusyWait is unspecified.
 * 
 * For longer delays it is much better to use the systemSleepMs. 
 * 
 * It is recommended that the main loop or other long
 * lasting loops calls systemSleep() once per turn.
 */
void sysBusyWait(uint32_t delay);
void sysSleepMs(int32_t timeMs);
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

// Not yet implemented.
#define sys_wdt_reset()

#endif
