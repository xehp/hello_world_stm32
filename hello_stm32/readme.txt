
This is a hello world program that can run on more than one type of STM32 MCU.


A compiler will be needed. Its possible that all you need to do is to install gcc-arm-none-eabi like this:
  sudo apt install gcc-arm-none-eabi





This program avoid RTOS and HAL but use the header files from HAL (those are good).
The header files are OK to distribute like this as far as I can understand.

To Compile do cd to desired device folder with a Makefile then:
make

NOTE if changing device/MCU do a make clean between since files may get mixed up.


If using Nucleo the device will show up as an USB flash drive when connected.
Just copy and paste the binary.bin file over. 

If all goes well it shall appear and then disappear after about 5 seconds.
If not try refresh. Just reboot the device after that.



st-link can be used to copy code to target via CLI (in long run much faster than mouse clicking).
  sudo apt install libusb-1.0 cmake
  cd ~/git/
  git clone https://github.com/texane/stlink.git
  cd stlink
  Read doc/compiling.md and compile it.

Download code to device (something like this, depending on installation of st-link):
sudo /home/henrik/git/stlink/build/st-flash --format ihex write binary.hex
sudo /home/henrik/git/stlink/build/Release/st-flash --format ihex write binary.hex
sudo /home/henrik/git/stlink/build/Debug/bin/st-flash --format ihex write binary.hex

For more see comments in "src/main.c".


See also github:
https://github.com/xehp/hello_world_stm32.git






So far only tested with Nucleo which has builtin ST-Link. 

According to RM0038 Rev 16 chapter 2.7 Boot configuration

Embedded boot loader
The embedded boot loader is used to reprogram the Flash memory through one of the
following interfaces:
In Cat.1 and Cat.2 devices: USART1 or USART2.
In Cat.3, Cat.4, Cat.5 and Cat.6 devices: USART1, USART2 or USB
This program is located in the system memory and is programmed by ST during production.

So only serial port connection with an STM32 should be needed to put a new program on it.





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
