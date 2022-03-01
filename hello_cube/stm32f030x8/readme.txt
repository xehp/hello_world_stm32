


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



Clock configuration
(hopefully no changes)


Project manager
Enter a name and location
Set Makefile as Toolchain / IDE

Press "Generate code"


Install GCC for arm
sudo apt install gcc-arm-none-eabi

cd to relevant folder.
make


Install stlink
st-flash --connect-under-reset --format ihex write build/hello_cube.hex


