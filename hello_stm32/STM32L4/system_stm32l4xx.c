/*
system_stm32l4xx.c

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

*/


#if (defined STM32L432xx)
#include "stm32l4xx.h"
#include "stm32l4xx_hal_cortex.h"
#else
#error Unknown target CPU/MCU
#endif

#include <inttypes.h>

#include "sys.h"




// TODO Would want to use the 8 MHz crystal and not the less accurate RC oscillator.
// https://community.st.com/s/question/0D50X00009XkgFp/usart-or-serial-trhough-stlinkv2-for-nucleo-stm32l152re
// The Nucleo boards don't have an external crystal, and the 8 MHz source from the ST-LINK must be made through the MCO (SB16 and SB50), which are normally not soldered.


#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */




/**
 * SystemInit
 * This is called from "startup_stm32l432xx.s" before main is called.
 * TODO This shall initialize the Embedded Flash Interface, the PLL and update the "SystemCoreClock" variable (whatever that is).
 */
void SystemInit(void)
{
	/* FPU settings */
	// TODO perhaps move this to sysVectorInit?
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif


	// Setup system clock
	#if SysClockFrequencyHz == 80000000U

		// TODO: This uses 16 HSI as base, it has 1% accuracy.
		// But we really want HSE from ST-Link.
		// If that does not work perhapås MSI can give 0.25% accuracy?

		// Initialize/Enable the HSI clock and wait until HSI stable
		RCC->CR |= RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}
		sysBusyWait(10);

		// Setup PLL so we can have 80 MHz


		// https://community.st.com/thread/41606-stm32-pll-clock-source-doesnt-work
		// Also note, that FLASH latency has to be set properly prior switching to a higher system clock, if running from FLASH.
		// [1] 3.3.3 Read access latency
		// [1] Bits 2:0 LATENCY[2:0]: Latency
		FLASH->ACR |= (4 << FLASH_ACR_LATENCY_Pos);

		// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
		// For now just busy wait
		sysBusyWait(10000);


		/* HSEBYP, external clock (from ST-Link, not chrystal) */
		/*
		  [1] Bit 18 HSEBYP: HSE crystal oscillator bypass
		  Set and cleared by software to bypass the oscillator with an external clock. The external
		  clock must be enabled with the HSEON bit set, to be used by the device. The HSEBYP bit
		  can be written only if the H
		  SE oscillator is disabled.
		  0: HSE crystal oscillator not bypassed
		  1: HSE crystal oscillator by
		  passed with external clock
		*/
		//RCC->CR &= ~(RCC_CR_HSEBYP_Msk);
		//RCC->CR |= RCC_CR_HSEBYP_Msk;
		// I have not gotten it to work with the external (HSE) clock.


		// First turn PLL off
		// And other clocks on.
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		RCC->CR |= RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_MSION_Msk;
		//LL_RCC_HSI_Enable();


		// Set the RCC_PLLCFGR
		{
			uint32_t tmpRCC_PLLCFGR = 0x00001000U; // (RCC_PLLCFGR register reset settings

			//uint32_t tmp = RCC->PLLCFGR;
			//tmpRCC_PLLCFGR &= ~(RCC_PLLCFGR_PLLREN_Msk | (0x7f << RCC_PLLCFGR_PLLN_Pos) | (3 <<RCC_PLLCFGR_PLLSRC_Pos));

			// TODO, Is this one needed? Or instead are more of these needed?
			// [1] Bit 24 PLLREN: Main PLL PLLCLK output enable
			tmpRCC_PLLCFGR |= RCC_PLLCFGR_PLLREN_Msk;

			const uint32_t wantedPLLN = 20U;
			tmpRCC_PLLCFGR |= wantedPLLN << RCC_PLLCFGR_PLLN_Pos;
			// Probably I want 10x PLL with External HSE 8 MHz.

			// Divide by 2
			const uint32_t wantedPLLM = 1;
			tmpRCC_PLLCFGR |= wantedPLLM << RCC_PLLCFGR_PLLM_Pos;

			/*
			[1] 6.4.4 PLL configuration register (RCC_PLLCFGR)
			Bits 1:0 PLLSRC: Main PLL and PLLSAI1 entry clock source
			Set and cleared by software to select PLL and PLLSAI1 clock source. These bits can be
			written only when PLL and PLLSAI1 are disabled.
			00: No clock sent to PLL and PLLSAI1
			01: MSI clock selected as PLL and PLLSAI1 clock entry
			10: HSI16 clock selected as PLL and PLLSAI1 clock entry
			11: HSE clock selected as PLL and PLLSAI1 clock entry
			 */
			const uint32_t wantedPllSrc = 0x2U;
			tmpRCC_PLLCFGR |= (wantedPllSrc << RCC_PLLCFGR_PLLSRC_Pos);

			// Using 16 MHz HSI, x20 /2 the system frequency should be:
			// 16 * 20 / 2 MHz = 160 MHz
			// But it seems I get the wanted 80 MHz with the setting.
			// Did I miss some additional divide by 2 somewhere?

			RCC->PLLCFGR = tmpRCC_PLLCFGR;
		}


		// Turn PLL on
		RCC->CR |= RCC_CR_PLLON;

		// TODO Wait for PLL to be stable, for now just busy wait.
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
			//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
		}


		sysBusyWait(10000);


		// Disable unused clocks.
		// This might have no effect on clocks still used?
		//RCC->CR &= ~(RCC_CR_HSION_Msk | RCC_CR_MSION_Msk);
		RCC->CR &= ~(RCC_CR_MSION_Msk | RCC_CR_HSEON_Msk);

	#elif SysClockFrequencyHz == 16000000U
		// Use HSI, 16 MHz, no PLL

		// [1] 3.3.3 Read access latency
		// [1] Bits 2:0 LATENCY[2:0]: Latency
		// TODO increasing latency here might not be needed, see [1] table 9.
		FLASH->ACR |= (1 << FLASH_ACR_LATENCY_Pos);

		// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
		// For now just busy wait
		sysBusyWait(10000);

		// Initialize/Enable the HSI clock and wait until HSI stable
		RCC->CR |= RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}
		sysBusyWait(10);


		/* Set HSI as SYSCLCK source */
		// Set system clock source to PLL.
		{
			// HSI16 selected as system clock
			// 01: HSI16 selected as system clock
			const uint32_t wantedClock = 1;
			//tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk << RCC_CFGR_SW_Pos); // Probably not needed since Reset value: 0x0000 0000
			RCC->CFGR |= (wantedClock << RCC_CFGR_SW_Pos);

			// Wait for system clock to be on.
			//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
			sysBusyWait(10000);
		}



	#elif SysClockFrequencyHz == 4000000U

		// Use MSI, 4 MHz, no PLL

		// Initialize the MSI clock
		// TODO This is probably not needed here.
		RCC->CR |= RCC_CR_MSION; // Enable HSI
		//while (!(RCC->CR & RCC_CR_MSIRDY_Msk))
		//{
		//	; // Wait until MSI stable
		//}
		sysBusyWait(10000);


		/* Set MSI as SYSCLCK source */
		// Set system clock source to PLL.
		{

			// HSI16 selected as system clock
			// 00: MSI selected as system clock
			// This is reset value so do nothing for now.
			//uint32_t tmpRCC_CFGR = 0;
			//const uint32_t wantedClock = 0;
			//tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk << RCC_CFGR_SW_Pos); // Probably not needed since Reset value: 0x0000 0000
			//tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
			//RCC->CFGR = tmpRCC_CFGR;

			// Wait for system clock to be on.
			//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
			sysBusyWait(10000);
		}


	#else
	#error // Not implemented SysClockFrequencyHz
	#endif

	// It seems to be good with a tiny delay here and there for clocks etc to start up.
	sysBusyWait(5);


	// The following shall be commented out when code works.
	// If something is not working? Try enable everything, might help or not.
	// Doing this or not should not be needed so comment this out normally.
	//RCC->AHB1ENR |= 0x00011103U;
	//RCC->AHB2ENR |= 0x0005209FU;
	//RCC->AHB3ENR |= 0x00000100U;
	//RCC->APB1ENR1 |= 0xF7EEC633U;
	//RCC->APB1ENR2 |= 0x00000003U;
	//RCC->APB2ENR |= 0x01235C01U;


	/* Configure the Vector Table location add offset address ------------------*/
	#ifdef VECT_TAB_SRAM
	SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
	#else
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
	#endif


}
