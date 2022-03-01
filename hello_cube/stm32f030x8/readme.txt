


Install STM32CubeMX

file:///home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/Readme.html
/home/henrik/Downloads/en.stm32cubemx-lin_v6.4.0/SetupSTM32CubeMX-6.4.0



Start my project from MCU

Have this one: STM32F030C8T6
Last two chars are only temp range.
So select: STM32F030C8


Pinout & Configuration

Enable IWDG
Set clock prescaler to max (256)

Under NVIC enable USART1 & USART2 interrupt.


USART1 & USART2 are used in mode Asynchronous.
No HW Flow control.


Clock configuration
(hopefully no changes)


Project manager
Enter a name and location
Set Makefile as Toolchain / IDE

Press "Generate code"


