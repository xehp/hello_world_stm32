/* 
GCC linker script

If you need to modify this file for another device it can be usefull to read more here:
http://www.stf12.org/developers/freerots_ec-linker_script.html
This file needs to match the hardware and the startup module, in this case "startup_stm32l431xx.s".
*/

/* 
Entry Point

This tells the linker what the first line of code is called in the startup module
(typically some assember code, in our example its in file "startup_stm32l431xx.s").
This section shall not need to be edited (unless you play with the startup module).
*/
ENTRY(Reset_Handler)

/*
Highest address of the user mode stack 

This shall have the value of RAM begin plus RAM length.
So for STM32 the value here shall be RAM start plus the amount of (normal) RAM.
The STOM32L432 has 48K normal RAM (it has an additional 16K of ECC RAM).
Only change the value here, the name "_estack" must remain same as used in the 
startup module.
*/
_estack = 0x2000C000; /* This need to match the actuall device */

/* 
Generate a link error if heap and stack don't fit into RAM
Typically these two does not require editing.
*/
_Min_Heap_Size = 0x400;  /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/*
Specify the memory areas

The length of repective memory is different between different devices.
To find these values look up the "Datasheet". 
For STOM32L432 its called "DS11453" or "stm32l431kc.pdf".
For some reason the ECC RAM is not adjasent to non ECC ram.
If your device have no ECC RAM then probably set RAM2 length to zero.
Or for fun experiment swap RAM1 and RAM2 to see if you can run
your program using the ECC memory.
*/
MEMORY
{
RAM1 (xrw)      : ORIGIN = 0x20000000, LENGTH = 48K
RAM2 (xrw)      : ORIGIN = 0x10000000, LENGTH = 16K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 256K
}

/* 
Define output sections

This section shall not need to be edited (unless you play with the startup module).
Hopefully the below code can be used same for all different types of STM32L4 devices.

If you find another link file you might need to comment out the file names
listed in section "Remove information from the standard libraries". We needed to
do that.
*/
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM1 AT> FLASH

  _sibss2 = LOADADDR(.bss2);

  /* RAM2 section 
  * 
  * IMPORTANT NOTE! 
  * If initialized variables will be placed in this section, 
  * the startup code needs to be modified to copy the init-values.  
  */
  .bss2 :
  {
    . = ALIGN(4);
    _sbss2 = .;       /* create a global symbol at ram2 start */
    *(.bss2)
    *(.bss2*)
    
    . = ALIGN(4);
    _ebss2 = .;       /* create a global symbol at ram2 end */
  }  >RAM2 AT> FLASH

  _sibss3 = LOADADDR(.bss3);

  .bss3 :
  {
    . = ALIGN(4);
    _sbss3 = .;       /* create a global symbol at ram3 start */
    *(.bss3)
    *(.bss3*)
    
    . = ALIGN(4);
    _ebss3 = .;       /* create a global symbol at ram3 end */
  } >RAM2 AT> FLASH
  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM1

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM1

  _Heap_Begin = _end;
  _Heap_Limit = _estack - _Min_Stack_Size;
  

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    /*libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )*/
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
