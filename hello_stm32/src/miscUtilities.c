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

#include "sys.h"
#include "cfg.h"
#include "miscUtilities.h"


static const int hex[]={'0','1','2','3','4','5','6','7', '8','9','A','B','C','D','E','F'};


/* A utility function to reverse a string  */
static void misc_reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
    	// swap(*(str+start), *(str+end));
    	char tmp=*(str+start);
    	*(str+start)=*(str+end);
    	*(str+end)=tmp;

        start++;
        end--;
    }
}

// base must be 1 thrue 16
char* misc_lltoa(int32_t num, char* str, int base)
{
	int i = 0;
	char isNegative = 0;

	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		const int r = num % base;
		str[i] = hex[r & 0xf];
		++i;
		num = num / base;
	}

	// If number is negative, append '-'
	if (isNegative)
	{
	    str[i++] = '-';
	}

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	misc_reverse(str, i);

	return str;
}


int utility_memcmp(const int8_t* ptr1, const int8_t* ptr2, int n)
{
	while(n>0)
	{
		const int8_t d = *ptr1 - *ptr2;
		if (d)
		{
			return d;
		}
		n--;
		ptr1++;
		ptr2++;
	}
	return 0;
}

void utility_memcpy(uint8_t *dst, const uint8_t *src, int n)
{
	while(n>0)
	{
		*dst = *src;
		dst++;
		src++;
		n--;
	}
}

int utility_strlen(const char *str)
{
	uint8_t n = 0;
	while(*str != 0)
	{
		str++;
		n++;
	}
	return n;
}

// TODO Make sure string is zero terminated, as snprintf would have done.
int utility_strccpy(uint8_t *dst, const uint8_t *src, int n)
{
	SYS_ASSERT(n>0);

	int c=0;
	while ((n>0) && (*src!=0))
	{
		*dst = *src;
		dst++;
		src++;
		n--;
		c++;
	}
	return c;
}



#if (defined __linux__) || (defined __WIN32) || (defined DEBUG_DECODE_DBF)

int utility_isgraph(int ch)
{
	return ((ch>' ') && (ch<=127));
}

int utility_isprint(int ch)
{
	return ((ch>=' ') && (ch<=127));
}

/* A utility function to reverse a string  */
// Used code from
// http://www.geeksforgeeks.org/implement-itoa/
static void utility_reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
    	// swap(*(str+start), *(str+end));
    	char tmp=*(str+start);
    	*(str+start)=*(str+end);
    	*(str+end)=tmp;

        start++;
        end--;
    }
}

// Returns number of characters written (not the pointer to src)
// NOTE! buffer must be large enough! 32 bytes should do, not less.
int utility_lltoa(int32_t num, char* bufptr, int base, int buflen)
{
    int32_t i = 0;
    char isNegative = 0;

    SYS_ASSERT(buflen>=32);

    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0)
    {
    	bufptr[i++] = '0';
    	bufptr[i] = '\0';
        return i;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int32_t rem = num % base;
        bufptr[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
    	bufptr[i++] = '-';

    bufptr[i] = '\0'; // Append string terminator

    // Reverse the string, since the above wrote it backwards.
    utility_reverse(bufptr, i);

    return i;
}



int32_t utility_atoll(const char* str)
{
    int32_t value = 0;

    while (*str == ' ')
    {
    	str++;
    }

    if (*str == '-')
    {
    	str++;
    	return -utility_atoll(str);
    }
    else if (*str == '+')
    {
    	str++;
	}

    while ((*str >= '0') && (*str <= '9'))
    {
    	value = value*10 + (*str -'0');
    	str++;
    }

    return value;
}
#endif

