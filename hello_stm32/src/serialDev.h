/*
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
*/

#ifndef USARTDEV_H
#define USARTDEV_H

#include "cfg.h"

enum
{
  DEV_USART1 = 1,
  DEV_USART2 = 2, // (on a Nucleo device this routed to USB)
  DEV_USART3 = 3,
#ifdef STM32L432xx
  DEV_LPUART1 = 11,
#else
#endif
};

void setupIoPinTx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction);
void setupIoPinRx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction);

int serialInit(int usartNr, uint32_t baud);
void serialPutChar(int usartNr, int ch);
int serialGetChar(int usartNr);
void serialWrite(int usartNr, const char *str, int msgLen);
void serialPrint(int usartNr, const char *str);
void serialPrintInt32(int usartNr, int32_t num);
int serialGetFreeSpaceWriteBuffer(int usartNr);

void serialPrintHex4(int usartNr, uint8_t num);
void serialPrintHex8(int usartNr, uint8_t num);
void serialPrintHex16(int usartNr, uint16_t num);
void serialPrintHex32(int usartNr, uint32_t num);

#define logLine(str) {serialPrint(DEV_USART1, str); serialPrint(DEV_USART2, str); serialPrint(DEV_USART1, "\r\n"); serialPrint(DEV_USART2, "\r\n"); sysSleepMs(100);}

#endif
