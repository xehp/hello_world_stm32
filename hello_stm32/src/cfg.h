/*
cfg.h

provide functions to set up hardware

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

History

2005-03-23
Created by Henrik Bjorkman

2018-07-15
Adapted for using STM32.


References
[7] DS9866.pdf
[8] UM1956 Rev 5
*/


#ifndef CFG_HDR_H_
#define CFG_HDR_H_

// Wanted system clock frequency
#if (defined STM32L432xx)
// Depending on clock configuration in SystemInit
// The following can be used 4000000U, 16000000U or 80000000U.
#define SysClockFrequencyHz 80000000U
#elif (defined STM32F303x8)
// Supported values are 8000000U, 36000000U or 64000000U.
#define SysClockFrequencyHz 64000000U
#else
// See also SystemCoreClock
#define SysClockFrequencyHz 8000000U
#endif


// Which GPIO pin to use for system/debug LED
#if (defined STM32L432xx)
// This is the Cortex M4 Lowpower version L4321
#define DEBUG_LED_PORT GPIOB
#define DEBUG_LED_PIN 3
#define DEBUG_LED_ACTIVE_HIGH
#elif (defined STM32L151xB)
// This is the Cortex M3 Lowpower version L151xx
// To know which macro to use see "Table 1. Macros for STM32L1 series" in DocID026721 Rev 3
// We have STM32L151C8T6, but T6 is package so look for STM32L151C8, so macro in stm32l1xx.h
// shall be: STM32L151xB
#define DEBUG_LED_PORT GPIOC
#define DEBUG_LED_PIN 13
#define DEBUG_LED_ACTIVE_LOW
#elif (defined STM32F303x8)
// STM32F303K8T6, Read more about this device: [7]
// Ref [8] chapter 6.5 "LEDs": User LD3: the green LED is a user LED connected to Arduino Nano signal D13
// corresponding to the STM32 I/O PB3 (pin 26).
// When the I/O is HIGH value, the LED is on
#define DEBUG_LED_PORT GPIOB
#define DEBUG_LED_PIN 3
#define DEBUG_LED_ACTIVE_HIGH
#else
#warning Unknown target MCU
#endif

// Which USARTs to use and at which baudrate.
#define USART1_BAUDRATE 115200
#define USART2_BAUDRATE 115200
//#define USART3_BAUDRATE 115200

// Support for Low Power Uart (LPUART)
//#define LPUART1_BAUDRATE 9600




#endif
