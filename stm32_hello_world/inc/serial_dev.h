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

#include "dev_cfg.h"
#include "sys_dev.h"

#define ASSERT_STR(s) #s
#define ASSERT_XSTR(s) ASSERT_STR(s)
#define ASSERT(c) {if (!(c)) {serialLogLine("assert " __FILE__ ":" ASSERT_XSTR(__LINE__)); sysErrorHandler(SYS_ASSERT_ERROR);}}

#ifndef STM32F103xB
void setupIoPinTx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction);
void setupIoPinRx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction);
#endif

#ifdef USART1_BAUDRATE
int serial1Init();
void serial1PutChar(int ch);
int serial1GetChar();
void serial1Write(const char *str, int msgLen);
void serial1Print(const char *str);
#endif

#ifdef USART2_BAUDRATE
int serial2Init();
void serial2PutChar(int ch);
int serial2GetChar();
void serial2Write(const char *str, int msgLen);
void serial2Print(const char *str);
#endif

#ifdef USART3_BAUDRATE
int serial3Init();
void serial3PutChar(int ch);
int serial3GetChar();
void serial3Write(const char *str, int msgLen);
void serial3Print(const char *str);
#endif

void serialLog(const char *str, int msgLen);
void serialLogLine(const char *str);

#endif
