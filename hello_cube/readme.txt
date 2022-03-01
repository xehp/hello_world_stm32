
This is a hello world program for stm32l151xb


A cross compiler will be needed. If not already installed just do:
sudo apt install gcc-arm-none-eabi


Before programming the chip pull BOOT0 high. 
Move jumper on J20 to connect pins 3 & 1, (the position closer to LDO).

cd stm32l151xb

make && st-flash --connect-under-reset --format ihex write build/hello_usb.hex

If all goes well it shall say: "Flash written and verified!".

After programming move BOOT0 jumper back to low, connect 3 & 5 (run position).




Copyright (C) 2022 Henrik Bjorkman www.eit.se/hb.

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



