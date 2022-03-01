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

2016-08-08 Adapted for atmega328p
2018-07-11 Adapted for STM32
2021-04-06 Used for current leak sensor also. /Henrik
2021-09-06 Changed format
*/


// Enter name and version numbers here:
#define VERSION_NAME "Drekkar HelloCubeStm32"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_DEBUG 5

// Don't edit these (only used internally in version.h):
#define VER_XSTR(s) VER_STR(s)
#define VER_STR(s) #s

#define VERSION_STRING VERSION_NAME " " VER_XSTR(VERSION_MAJOR) "." VER_XSTR(VERSION_MINOR) "." VER_XSTR(VERSION_DEBUG)
