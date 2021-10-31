
This is a hello world program that can run on more than one type of STM32 MCU.

This program avoid complications such as RTOS and HAL but use the header files 
from HAL (those are good). Those header files are OK to distribute like this
as far as I can understand.

In each of the folders STM32F3, STM32L4 etc there is a "readme.txt" file
explaing how to adapt the code for other devices than the one tested (by us).

A cross compiler will be needed. If not already installed just do:
sudo apt install gcc-arm-none-eabi

If your device is STM32F3 then read more in STM32F3/readme.txt

To Compile do cd to desired device folder (STM32F3, STM32L4 etc) then:
make

NOTE if changing device/MCU do a make clean between since files may get mixed up.

If using Nucleo the device will show up as an USB flash drive when connected.
Just copy and paste the "binary.bin" that is generated file over to that drive. 

If all goes well it (the file) shall appear and then disappear after about 5 seconds.
If not try refresh and/or reboot the device.




https://github.com/xehp/hello_world_stm32.git

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



