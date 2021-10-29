# hello_world_stm32


This is a hello world program for Nucleo Nucleo L432KC.
The intention is to be able run on more than one type of STM32 MCU.


A compiler will be needed. Its possible that all you need to do is to install gcc-arm-none-eabi like this:

sudo apt install gcc-arm-none-eabi


This program avoid RTOS and HAL but use the header files from HAL (those are good).

To Compile do cd to desired device folder with a Makefile then just:

make


NOTE if changing device/MCU do a make clean between since files may get mixed up, so:

make clean


If using Nucleo the device will show up as an USB flash drive when connected.
Just copy and paste the binary.bin file over. 

If all goes well it shall appear and then disappear after about 5 seconds.
If not try refresh. Just reboot the device after that.




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
