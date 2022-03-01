/*
fifo.h

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

Created 2019 by Henrik
*/



#ifndef FIFO_H
#define FIFO_H

#include "string.h"

// FIFO Size must be 256 bytes as we depend on uint8_t to wrap by itself.
#define FIFO_BUFFER_SIZE 256

// In this FIFO entries are put in head and taken from tail.
struct Fifo
{
	uint8_t head;
	uint8_t tail;
	char buffer[FIFO_BUFFER_SIZE];
};




static inline void fifoInit(volatile struct Fifo *fifoPtr)
{
	memset((void*)fifoPtr, 0, sizeof(*fifoPtr));
}

static inline void fifoPut(volatile struct Fifo *fifoPtr, char ch)
{
	fifoPtr->buffer[fifoPtr->head] = ch;
	fifoPtr->head++;
}

static inline int fifoIsFull(volatile struct Fifo *fifoPtr)
{
	return (((uint8_t)(fifoPtr->head+1)) == fifoPtr->tail);
}

static inline int fifoIsEmpty(volatile struct Fifo *fifoPtr)
{
	return (fifoPtr->head == fifoPtr->tail);
}

static inline char fifoTake(volatile struct Fifo *fifoPtr)
{
	const char tmp = fifoPtr->buffer[fifoPtr->tail++];
	return tmp;
}

static inline int fifo_get_bytes_in_buffer(volatile struct Fifo *fifoPtr)
{
	return (fifoPtr->head - fifoPtr->tail) & (FIFO_BUFFER_SIZE-1);
}

static inline int fifo_free_space(volatile struct Fifo *fifoPtr)
{
	return((FIFO_BUFFER_SIZE-1)-fifo_get_bytes_in_buffer(fifoPtr));
}

#endif

