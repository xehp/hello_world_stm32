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
#include <inttypes.h>


/* A utility function to reverse a string  */
void utility_reverse(char str[], size_t length);

int utility_lltoa(int64_t num, char* str, int base);

int utility_memcmp(const char* ptr1, const char* ptr2, size_t n);
void utility_memcpy(char *dst, const char *src, size_t n);
int utility_strlen(const char *str);
int utility_strccpy(char *dst, const char *src, size_t n);

#if (defined __linux__) || (defined __WIN32) || (defined DEBUG_DECODE_DBF)

int utility_isgraph(int ch);
int utility_isprint(int ch);

int utility_lltoa(int64_t num, char* bufptr, int base, size_t buflen);

int64_t utility_atoll(const int8_t* str);

#endif

#endif
