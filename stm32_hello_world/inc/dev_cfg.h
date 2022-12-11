/*
dev_cfg.h

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


#ifndef STM32_CFG_HDR_H_
#define STM32_CFG_HDR_H_

#include "version.h"

//#define SysClockFrequencyHz SystemCoreClock

#if (VERSION_PCB <= 3)
#define DEBUG_LED_PORT GPIOB
#define DEBUG_LED_PIN 3
#define DEBUG_LED_ACTIVE_LOW
#elif (VERSION_PCB >= 4) && (VERSION_PCB <= 6)
// SYS LED did not work on these.
#elif (VERSION_PCB ==7)
#define DEBUG_LED_PORT GPIOA
#define DEBUG_LED_PIN 5
#define DEBUG_LED_ACTIVE_HIGH
#elif (VERSION_PCB == 8)
#define DEBUG_LED_PORT GPIOA
#define DEBUG_LED_PIN 5
#define DEBUG_LED_ACTIVE_LOW
#else
#error
#endif

#if (VERSION_PCB <= 5) || (VERSION_PCB >= 8)
#define USART1_BAUDRATE 115200
#endif

#define USART2_BAUDRATE 115200

#if VERSION_PCB == 5
#define USART3_BAUDRATE 115200
#endif


#ifdef USART2_BAUDRATE
#define CONSOLE_DEV 2
#elif defined USART1_BAUDRATE
#define CONSOLE_DEV 1
#endif




#endif

