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

#include <stddef.h>
#include "dev_cfg.h"
#include "sys_dev.h"
#include "utility_functions.h"

static const int hex[]={'0','1','2','3','4','5','6','7', '8','9','A','B','C','D','E','F'};

int utility_encode_digit(uint8_t d)
{
	//return (d > 9) ? (d - 10) + 'a' : d + '0';
	return hex[d & 0xF];
}

int utility_decode_digit(int ch)
{
	if ((ch >= '0') && (ch <= '9'))
	{
		return ch - '0';
	}
	if ((ch >= 'a') && (ch <= 'f'))
	{
		return ch + 10 - 'a';
	}
	if ((ch >= 'A') && (ch <= 'F'))
	{
		return ch + 10 - 'A';
	}
	return -1;
}

// Copy a string while making sure string is zero terminated, as snprintf would have done.
// Returns number of characters copied (not counting the terminating zero).
int utility_strccpy(char *dst, const char *src, size_t n)
{
	int c = 0;
	if (n > 0)
	{
		--n; // reserve room for terminating zero

		while ((n>0) && (*src!=0))
		{
			*dst = *src;
			++dst;
			++src;
			--n;
			++c;
		}
		*dst = 0;
	}
	return c;
}


/* A utility function to reverse a string  */
void utility_reverse(char str[], size_t length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
    	// swap(*(str+start), *(str+end));
    	const char tmp=*(str+start);
    	*(str+start)=*(str+end);
    	*(str+end)=tmp;

        start++;
        end--;
    }
}

// This returns number of bytes written (not including terminating zero).
// NOTE standard itoa returns a pointer.
int utility_itoa(int num, char* str, int base)
{
	int n = 0;
	char isNegative = 0;

	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[n++] = '0';
		str[n] = '\0';
		return n;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if ((num < 0) && (base == 10))
	{
		isNegative = 1;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		const int rem = num % base;
		str[n++] = utility_encode_digit(rem);
		num = num/base;
	}

	// If number is negative, append '-'
	if (isNegative) {str[n++] = '-';}

	str[n] = '\0'; // Append string terminator

	// Reverse the string
	utility_reverse(str, n);

	return n;
}

