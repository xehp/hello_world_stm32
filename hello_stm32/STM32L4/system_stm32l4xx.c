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

		// [RM0394 Rev 4] 5.1.7 Dynamic voltage scaling management
		// Need the main regulator to provide 1.2 V
		// That should be the reset value.

		// This uses 16 HSI as base, it has 1% accuracy.
		// If that is not good enough perhaps MSI can give 0.25% accuracy?

		// First turn PLL off and HSI on.
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}
		sysBusyWait(1000);

		// Temporarily set HSI 16 MHz as SYSCLCK source
		// HSI16 selected as system clock
		// 01: HSI16 selected as system clock
		RCC->CFGR |= (1 << RCC_CFGR_SW_Pos);
		sysBusyWait(1000);

		// Also note, that FLASH latency has to be set properly prior switching to a higher system clock, if running from FLASH.
		// [1] 3.3.3 Read access latency
		// [1] Bits 2:0 LATENCY[2:0]: Latency
		const uint32_t wanted_latency = 4U;
		FLASH->ACR |= (wanted_latency << FLASH_ACR_LATENCY_Pos);

		// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
		while ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != (wanted_latency << FLASH_ACR_LATENCY_Pos)) {sysBusyWait(10);}
		sysBusyWait(100);





		// Setup PLL so we can have 80 MHz

		#if SysClockExternalHz == 0U

		// Set the RCC_PLLCFGR
		{
			uint32_t tmpRCC_PLLCFGR = 0;

			//uint32_t tmp = RCC->PLLCFGR;
			//tmpRCC_PLLCFGR &= ~(RCC_PLLCFGR_PLLREN_Msk | (0x7f << RCC_PLLCFGR_PLLN_Pos) | (3 <<RCC_PLLCFGR_PLLSRC_Pos));

			// TODO, Is this one needed? Or instead are more of these needed?
			// [1] Bit 24 PLLREN: Main PLL PLLCLK output enable
			tmpRCC_PLLCFGR |= RCC_PLLCFGR_PLLREN_Msk;

			// VCO output frequency = VCO input frequency x PLLN (with 8 =< PLLN =< 86)
			const uint32_t wantedPLLN = 20U;
			tmpRCC_PLLCFGR |= wantedPLLN << RCC_PLLCFGR_PLLN_Pos;

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


			// TODO Adjust this one (and others) to give 48 MHz needed by USB etc.
			// Setting this or not should not matter for now since we have not set PLLQEN
			//const uint32_t wantedPLLQ = 3;
			//tmpRCC_PLLCFGR |= wantedPLLQ << RCC_PLLCFGR_PLLQ_Pos;

			// Setting this or not should not matter for now since we have not set PLLPEN
			//const uint32_t wantedPLLP = 1;
			//tmpRCC_PLLCFGR |= wantedPLLP << RCC_PLLCFGR_PLLP_Pos;


			RCC->PLLCFGR = tmpRCC_PLLCFGR;
		}

		#else
		#error
		#endif

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
		RCC->CR &= ~(RCC_CR_MSION_Msk | RCC_CR_HSEON_Msk);

	#elif (SysClockFrequencyHz == 64000000U)

		// As mentioned in 32000000U MHz section did not get HSE working. It seems to need a solder
		// bridge closed, that is HW modification. However If we enable 32768 KHz we can configure
		// MSI to be better than ±0.25% accuracy.
		//
		// [DS11453 Rev 3] 3.11 "Clocks and startup"
		// Multispeed internal RC oscillator (MSI), trimmable by software, able to generate
		// 12 frequencies from 100 kHz to 48 MHz. When a 32.768 kHz clock source is
		// available in the system (LSE), the MSI frequency can be automatically trimmed by
		// hardware to reach better than ±0.25% accuracy. The MSI can supply a PLL.
		//
		// TODO figure out how to do that "automatically trimmed by hardware".
		//
		// [RM0394 Rev 4] 6.2.6 LSE clock
		//RCC->BDCR |= RCC_BDCR_LSEON_Msk;
		//while (!(RCC->BDCR & RCC_BDCR_LSERDY_Msk)) {sysBusyWait(1);}
		//
		//RCC->CR |= RCC_CR_MSIPLLEN_Msk;
		//
		// TODO Perhaps change MSI to 48 MHz to be used by USB
		//RCC->CR &= ~RCC_CR_MSIRANGE_Msk;
		//RCC->CR |= 0b1011 << RCC_CR_MSIRANGE_Pos;

		// First turn PLL off and HSI on.
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}
		sysBusyWait(1000);

		// Temporarily set HSI 16 MHz as SYSCLCK source
		// HSI16 selected as system clock
		// 01: HSI16 selected as system clock
		RCC->CFGR |= (1 << RCC_CFGR_SW_Pos);
		sysBusyWait(1000);

		// Also note, that FLASH latency has to be set properly prior switching to a higher system clock, if running from FLASH.
		// [1] 3.3.3 Read access latency
		// [1] Bits 2:0 LATENCY[2:0]: Latency
		const uint32_t wanted_latency = 3U;
		FLASH->ACR |= (wanted_latency << FLASH_ACR_LATENCY_Pos);

		// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
		while ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != (wanted_latency << FLASH_ACR_LATENCY_Pos)) {sysBusyWait(10);}
		sysBusyWait(100);

		#if SysClockExternalHz == 0U

		// Set the RCC_PLLCFGR
		{
			uint32_t tmpRCC_PLLCFGR = 0;

			//uint32_t tmp = RCC->PLLCFGR;
			//tmpRCC_PLLCFGR &= ~(RCC_PLLCFGR_PLLREN_Msk | (0x7f << RCC_PLLCFGR_PLLN_Pos) | (3 <<RCC_PLLCFGR_PLLSRC_Pos));

			// TODO, Is this one needed? Or instead are more of these needed?
			// [1] Bit 24 PLLREN: Main PLL PLLCLK output enable
			tmpRCC_PLLCFGR |= RCC_PLLCFGR_PLLREN_Msk;

			// VCO output frequency = VCO input frequency x PLLN (with 8 =< PLLN =< 86)
			const uint32_t wantedPLLN = 64U;
			tmpRCC_PLLCFGR |= wantedPLLN << RCC_PLLCFGR_PLLN_Pos;

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
			const uint32_t wantedPllSrc = 1U;
			tmpRCC_PLLCFGR |= (wantedPllSrc << RCC_PLLCFGR_PLLSRC_Pos);

			// TODO Adjust this one (and others) to give 48 MHz needed by USB etc.
			// Setting this or not should not matter for now since we have not set PLLQEN
			//const uint32_t wantedPLLQ = 3;
			//tmpRCC_PLLCFGR |= wantedPLLQ << RCC_PLLCFGR_PLLQ_Pos;

			// Setting this or not should not matter for now since we have not set PLLPEN
			//const uint32_t wantedPLLP = 1;
			//tmpRCC_PLLCFGR |= wantedPLLP << RCC_PLLCFGR_PLLP_Pos;


			RCC->PLLCFGR = tmpRCC_PLLCFGR;

			// Using 4 MHz MSI, x32 /2 the system frequency should be:
			// 4 * 32 / 2 MHz = 64 MHz
			// But it seems I get the wanted 32 MHz with the setting.
			// Did I miss some additional divide by 2 somewhere?

		}

		#else
		#error
		#endif


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

		// TODO Disable unused clocks to save power.
		RCC->CR &= ~(RCC_CR_HSION_Msk | RCC_CR_MSION_Msk);


	#elif (SysClockFrequencyHz == 32000000U)


		// First turn PLL off and HSI on.
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}
		sysBusyWait(1000);

		// Temporarily set HSI 16 MHz as SYSCLCK source
		// HSI16 selected as system clock
		// 01: HSI16 selected as system clock
		RCC->CFGR |= (1 << RCC_CFGR_SW_Pos);
		sysBusyWait(1000);

		// Also note, that FLASH latency has to be set properly prior switching to a higher system clock, if running from FLASH.
		// [1] 3.3.3 Read access latency
		// [1] Bits 2:0 LATENCY[2:0]: Latency
		const uint32_t wanted_latency = 1U;
		FLASH->ACR |= (wanted_latency << FLASH_ACR_LATENCY_Pos);

		// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
		while ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != (wanted_latency << FLASH_ACR_LATENCY_Pos)) {sysBusyWait(10);}
		sysBusyWait(100);



		#if (SysClockExternalHz == 8000000U)

		#warning // When testing this code we got only 4 MHz, it seems MSI is still used although PLL/HSE is wanted.
		// TODO I have not gotten it to work with the external (HSE) clock.
		// Perhaps this explains what to do:
		// https://community.st.com/s/question/0D53W000000Yln1/is-there-external-oscillator-in-the-nucleol432kc-board-the-website-says-it-is-there-but-i-am-not-able-to-enable-hse-mode
		// Solder Bridge, basically a link option using solder instead of a push on pin arrangement.
		// Use SB17 as a search term in the User Manual / Schematic so you get the details / description.​
		// So on other HW this might work fine, if so perhaps do not use RCC_CR_HSEBYP_Msk below.
		// RCC->CR |= RCC_CR_HSEBYP_Msk

		// Setup PLL so we can have 32 MHz

		/* HSEBYP, external clock (from ST-Link, not crystal) */
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
		sysBusyWait(100000);
		RCC->CR |= RCC_CR_HSEON_Msk /*| RCC_CR_HSEBYP_Msk*/;
		// When tested this gave eternal loop, so HSE is never ready?
		// If this is commented out then it does not switch to HSE/PLL, it stays on HSI.
		// So what more is required to use HSE?
		/*while ((RCC->CR & RCC_CR_HSERDY_Msk) == 0)
		{
			sysBusyWait(10);
		}*/
		sysBusyWait(1000000);

		// Set the RCC_PLLCFGR
		{
			uint32_t tmpRCC_PLLCFGR = 0;

			//uint32_t tmp = RCC->PLLCFGR;
			//tmpRCC_PLLCFGR &= ~(RCC_PLLCFGR_PLLREN_Msk | (0x7f << RCC_PLLCFGR_PLLN_Pos) | (3 <<RCC_PLLCFGR_PLLSRC_Pos));

			// TODO, Is this one needed? Or instead are more of these needed?
			// [1] Bit 24 PLLREN: Main PLL PLLCLK output enable
			tmpRCC_PLLCFGR |= RCC_PLLCFGR_PLLREN_Msk;

			// Multiply input clock
			const uint32_t wantedPLLN = 16U;
			tmpRCC_PLLCFGR |= wantedPLLN << RCC_PLLCFGR_PLLN_Pos;

			// Divide by 2
			const uint32_t wantedPLLM = 1;
			tmpRCC_PLLCFGR |= wantedPLLM << RCC_PLLCFGR_PLLM_Pos;

			// TODO Adjust this one (and others) to give 48 MHz needed by USB etc.
			// Setting this or not should not matter for now since we have not set PLLQEN
			//const uint32_t wantedPLLQ = 3;
			//tmpRCC_PLLCFGR |= wantedPLLQ << RCC_PLLCFGR_PLLQ_Pos;

			// Setting this or not should not matter for now since we have not set PLLPEN
			//const uint32_t wantedPLLP = 1;
			//tmpRCC_PLLCFGR |= wantedPLLP << RCC_PLLCFGR_PLLP_Pos;


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
			const uint32_t wantedPllSrc = 0x3U;
			tmpRCC_PLLCFGR |= (wantedPllSrc << RCC_PLLCFGR_PLLSRC_Pos);


			RCC->PLLCFGR = tmpRCC_PLLCFGR;
		}

		#else
		#error
		#endif


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

		// TODO Disable unused clocks to save power.
		RCC->CR &= ~(RCC_CR_HSION_Msk | RCC_CR_MSION_Msk);

	#elif (SysClockFrequencyHz == 16000000U)

		#if SysClockExternalHz == 0U

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
			while (!(RCC->CR & RCC_CR_HSIRDY_Msk)) {sysBusyWait(10);}
			sysBusyWait(1000);

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
				sysBusyWait(1000);
			}
		#else
		#error
		#endif

	#elif SysClockFrequencyHz == 8000000U

		#if SysClockExternalHz == 8000000U

		#warning // When testing this it seems MSI is still used although HSE is wanted.
		// Perhaps this explains what to do:
		// https://community.st.com/s/question/0D53W000000Yln1/is-there-external-oscillator-in-the-nucleol432kc-board-the-website-says-it-is-there-but-i-am-not-able-to-enable-hse-mode
		// Solder Bridge, basically a link option using solder instead of a push on pin arrangement.
		// Use SB17 as a search term in the User Manual / Schematic so you get the details / description.​
		// So on other HW this might work fine, if so perhaps do not use RCC_CR_HSEBYP_Msk below.
		// RCC->CR |= RCC_CR_HSEBYP_Msk

		// Initialize the HSE clock and wait for it to start.
		// When I tested it never got ready. So eternal loop here, see comments above.
		RCC->CR |= RCC_CR_HSEON_Msk;
		while ((RCC->CR & RCC_CR_HSERDY_Msk) == 0) {sysBusyWait(10);}
		sysBusyWait(10000);

		// 10: HSE selected as system clock
		RCC->CFGR |= (2 << RCC_CFGR_SW_Pos);
		sysBusyWait(1000);

		#else
		#error
		#endif

	#elif SysClockFrequencyHz == 4000000U

		// TODO If using a clock at this low frequency it probably wanted to also reduce voltage to save power.
		// [RM0394 Rev 4] 5.1.7 Dynamic voltage scaling management

		#if SysClockExternalHz == 0U

		// Use MSI, 4 MHz, no PLL, this is reset setting so nothing to do.

		#else
		#error
		#endif

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
