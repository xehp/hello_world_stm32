/*
system_stm32f3xx.c

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

References
[1] DocID022558 Rev 8, RM0316, Reference manual
[2] PM0214 Rev 10
*/


#if (defined STM32F303x8)
#include "stm32f3xx.h"
#include "stm32f3xx_hal_cortex.h"
#else
#error Unknown target CPU/MCU
#endif

#include <inttypes.h>

#include "sys.h"

// Macros for backward compatibility, can be removed later.
#ifndef FLASH_ACR_LATENCY_Pos
// [1] 4.5.1 Flash access control register (FLASH_ACR)
#define FLASH_ACR_LATENCY_Pos 0
#endif
#ifndef RCC_CR_HSEON_Msk
#define RCC_CR_HSEON_Msk RCC_CR_HSEON
#endif
#ifndef RCC_CR_HSION_Msk
#define RCC_CR_HSION_Msk RCC_CR_HSION
#endif
#ifndef RCC_CR_PLLON_Msk
#define RCC_CR_PLLON_Msk RCC_CR_PLLON
#endif
#ifndef RCC_CR_PLLRDY_Msk
#define RCC_CR_PLLRDY_Msk RCC_CR_PLLRDY
#endif
#ifndef RCC_CFGR_PLLMUL_Pos
// [1] 9.4.2 "Clock configuration register (RCC_CFGR)"
// Bits 21:18 PLLMUL: PLL multiplication factor
#define RCC_CFGR_PLLMUL_Pos 18
#endif
#ifndef RCC_CFGR_SW_Pos
#define RCC_CFGR_SW_Pos 0
#endif
#ifndef RCC_CFGR_SW_Msk
#define RCC_CFGR_SW_Msk RCC_CFGR_SW
#endif


// TODO Would want to use the 8 MHz crystal and not the less accurate RC oscillator.
// https://community.st.com/s/question/0D50X00009XkgFp/usart-or-serial-trhough-stlinkv2-for-nucleo-stm32l152re
// The Nucleo boards don't have an external crystal, and the 8 MHz source from the ST-LINK must be made through the MCO (SB16 and SB50), which are normally not soldered.



/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field.
                                  This value must be a multiple of 0x200. */




/**
 * SystemInit
 * This is called from "startup_stm32l432xx.s" before main is called.
 * TODO This shall initialize the Embedded Flash Interface, the PLL and update the "SystemCoreClock" variable (whatever that is).
 */
void SystemInit(void)
{
	sysPinOutInit(SYS_LED_PORT, SYS_LED_PIN);
	sysPinOutSetHigh(SYS_LED_PORT, SYS_LED_PIN);

	/* FPU settings */
	// TODO perhaps move this to sysVectorInit?
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif

	// Setup system clock
	#if (SysClockFrequencyHz == 36000000) || (SysClockFrequencyHz == 64000000)



	// Setup PLL so we can have 72 MHz (but for now we do 36 Mhz or 64 MHz).
	// Ref [1] Chapter 9.2.3 "PLL"

	// TODO: This uses HSI as base, it has 1% accuracy. But we really want to use HSE (if available).

	// Initialize/enable the HSI clock and wait until HSI stable
	RCC->CR |= RCC_CR_HSION; // Enable HSI
	sysBusyWait(1000);

	// https://community.st.com/thread/41606-stm32-pll-clock-source-doesnt-work
	// Also note, that FLASH latency has to be set properly prior switching to a higher system clock, if running from FLASH.
	// [1] 4.5.1 Flash access control register (FLASH_ACR)
	// So we need:
	// 001: One wait state, if 24 MHz < HCLK ≤ 48 MHz
	// 010: Two wait states, if 48 < HCLK <= 72 MHz
	#if (SysClockFrequencyHz <= 48000000)
	const int latency = 1;
	#else
	const int latency = 2;
	#endif
	FLASH->ACR |= (latency << FLASH_ACR_LATENCY_Pos);

	// Ref [1] Chapter 9.2.3 "PLL" step 1. Disable the PLL by setting PLLON to 0
	// First turn other clocks on and then PLL off.
	RCC->CR |= RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk;
	RCC->CR &= ~RCC_CR_PLLON_Msk;

	// Ref [1] Chapter 9.2.3 "PLL" step 2. Wait until PLLRDY is cleared.
	while (RCC->CR & RCC_CR_PLLRDY_Msk)
	{
		sysBusyWait(10);
	}
	// The PLL is now fully stopped.

	// Ref [1] Chapter 9.2.3 "PLL" step 3. Change the desired parameter.
	// Set the RCC_CFGR, see ref [1] Chapter 9.4.2 "Clock configuration register (RCC_CFGR)"
	{
		uint32_t tmpRCC_CFGR = RCC->CFGR;

		// TODO Since PLLSRC is set to HSI/2 selected as PLL input clock
		// and ref [1] chapter 9.2 "Clocks" say "HSI 8 MHZ RC oscillator clock"
		// So using x 9 we should get 36 Mhz and x 16 shall give 64 MHz
		// 0111: PLL input clock x 9
		// 1110: PLL input clock x 16
		#if (SysClockFrequencyHz == 36000000)
		const uint32_t wantedPLLMUL = 7U;
		#elif (SysClockFrequencyHz == 64000000)
		const uint32_t wantedPLLMUL = 0xEU;
		#else
		#error
		#endif
		tmpRCC_CFGR |= wantedPLLMUL << RCC_CFGR_PLLMUL_Pos;

		// 0: HSI/2 selected as PLL input clock
		// That is the reset setting so no action needed.
		// const uint32_t wantedPLLSRC = 0;
		// tmpRCC_CFGR |= wantedPLLSRC << RCC_CFGR_PLLSRC_Pos;

		RCC->CFGR = tmpRCC_CFGR;
	}


	// Ref [1] Chapter 9.2.3 "PLL" step 4. Enable the PLL again by setting PLLON to 1.
	// And wait for it to be stable
	RCC->CR |= RCC_CR_PLLON_Msk;
	sysBusyWait(10000);

	// Set system clock source to PLL.
	{
		uint32_t tmpRCC_CFGR = 0;

		// PLL selected as system clock
		const uint32_t wantedClock = RCC_CFGR_SW_PLL;
		tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
		tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
		RCC->CFGR = tmpRCC_CFGR;

		// Wait for system clock to be on.
		// TODO while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
		sysBusyWait(10000);
	}


	sysBusyWait(1000);


	// TODO Disable unused clocks.
	// This might have no effect on clocks still used?
	//RCC->CR &= ~(RCC_CR_MSION_Msk | RCC_CR_HSEON_Msk);



	#elif SysClockFrequencyHz == 8000000
	// Use HSI, 8 MHz, no PLL, think this is default.

	// Increasing latency here should not be needed, see [1] 4.5.1 "Flash access control register (FLASH_ACR)".
	// 000: Zero wait state, if 0 < HCLK ≤ 24 MHz

	// Initialize/enable the HSI clock and wait until HSI stable
	RCC->CR |= RCC_CR_HSION;
	sysBusyWait(10000);

	// Nothing to do HSI is reset value (0)
	// 00: HSI selected as system clock


	// Now follows some code copied from
	// https://github.com/PaxInstruments/STM32CubeF3/blob/master/Projects/STM32F303K8-Nucleo/Templates/Src/system_stm32f3xx.c
	// Do not know if needed.

	  /* Reset CFGR register */
	  RCC->CFGR &= 0xF87FC00C;

	  /* Reset HSEON, CSSON and PLLON bits */
	  RCC->CR &= (uint32_t)0xFEF6FFFF;

	  /* Reset HSEBYP bit */
	  RCC->CR &= (uint32_t)0xFFFBFFFF;

	  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
	  RCC->CFGR &= (uint32_t)0xFF80FFFF;

	  /* Reset PREDIV1[3:0] bits */
	  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

	  /* Reset USARTSW[1:0], I2CSW and TIMs bits */
	  RCC->CFGR3 &= (uint32_t)0xFF00FCCC;

	  /* Disable all interrupts */
	  RCC->CIR = 0x00000000;

	#ifdef VECT_TAB_SRAM
	  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
	#else
	  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
	#endif



	#else
	#error Selected SysClockFrequencyHz is not (yet) implemented
	#endif

	sysBusyWait(10000);

	// Set system clock source to PLL.
	{
		// 10: PLL used as system clock
		const uint32_t wantedSW = 2U;
		RCC->CFGR |= (wantedSW << RCC_CFGR_SW_Pos);
	}


	sysBusyWait(10000);


	// Disable unused clocks.
	// This might have no effect on clocks still used?
	RCC->CR &= ~(RCC_CR_HSEON_Msk);

	// It seems to be good with a tiny delay here and there for clocks etc to start up.
	sysBusyWait(5);
}

