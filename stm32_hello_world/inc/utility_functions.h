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
#ifndef MISC_UTILITIES_H_
#define MISC_UTILITIES_H_

// TODO Do we need both of these includes?
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <limits.h>


// http://www.cplusplus.com/faq/sequences/arrays/sizeof-array/#cpp
#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] ))



// This shall give a/b but result is rounded up not down as normal
// Negative numbers? TODO is this correct for negative values?
#define DIV_ROUND(a,b) (((a)+((b)/2))/(b))

#define ABS(a) ( ((a) < 0) ? (-(a)) : (a))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


/* A utility function to reverse a string  */
void utility_reverse(char str[], size_t length);
int utility_strccpy(char *dst, const char *src, size_t n);
int utility_itoa(int num, char* str, int base);


#endif
