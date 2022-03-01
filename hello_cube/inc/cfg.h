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


#ifndef STM32_CFG_HDR_H_
#define STM32_CFG_HDR_H_

#define SysClockFrequencyHz SystemCoreClock

#if (defined STM32L151xB)

// Which GPIO pin to use for the LED that show if MCU is OK
// Usually it is PB3 but on our PCB it is PC13.
#define DEBUG_LED_PORT GPIOC
#define DEBUG_LED_PIN 13
#define DEBUG_LED_ACTIVE_LOW


// Which USARTs to use and at which baudrate (if commented out that USART is not used).
#define USART1_BAUDRATE 115200
#define USART2_BAUDRATE 115200
//#define USART3_BAUDRATE 115200

// Support for Low Power Uart (LPUART)
//#define LPUART1_BAUDRATE 9600


#define USB_ENABLE

#else

// Which GPIO pin to use for the LED that show if MCU is OK
// Usually it is PB3.
#define DEBUG_LED_PORT GPIOB
#define DEBUG_LED_PIN 3
#define DEBUG_LED_ACTIVE_LOW

#define USART1_BAUDRATE 115200
#define USART2_BAUDRATE 115200



#endif

#endif

