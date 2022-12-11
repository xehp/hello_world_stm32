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
#ifndef VERSION_H_
#define VERSION_H_

// Don't edit these:
#define VER_HSTR(s) #s
#define VER_XSTR(s) VER_HSTR(s)


// HW GENERATION OR VARIANT
// 1  2022-02-07
// 2  2022-03-25
// 3  2022-09-12 (input power voltage is available, ADC1_IN5  PA0)
// 4  rs-232_opto_adapter_voltage_smd_2022-08-02
// 5  rs-232_opto_adapter_voltage_smd 2022-10-28
// 6  RCLD_CAN_SN65HVD230DR 2022-10-16
// 7  RCLD_CAN_MK2 2022-11-23
// 8  NUCLEO_F103RB
#ifndef VERSION_PCB
#define VERSION_PCB 4
#endif



//#define APPLICATION_MAGIC_NUMBER 5556

// Enter name and version numbers here:
#define VERSION_NAME "STM32_hello_world"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_DEBUG 3


#define VERSION_STRING VERSION_NAME ":" VER_XSTR(VERSION_MAJOR) "." VER_XSTR(VERSION_MINOR) "." VER_XSTR(VERSION_DEBUG) ":" VER_XSTR(VERSION_PCB)

#endif
