// system_stm32l1xx.c

#include "stm32l1xx.h"
#include "cfg.h"
#include "sys.h"



  
#define VECT_TAB_OFFSET  0x0 /* This value must be a multiple of 0x200. */

#if defined (STM32L151xD) || defined (STM32L152xD) || defined (STM32L162xD)
#ifdef DATA_IN_ExtSRAM
#error not implemented
#endif
#endif


void SystemInit (void)
{
#if SysClockFrequencyHz == 2097000U

  /*!< Set MSION bit (perhaps not needed, it should already be on) */
  RCC->CR |= (uint32_t)RCC_CR_MSION_Msk;

  /*!< Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], MCOSEL[2:0] and MCOPRE[2:0] bits */
  //RCC->CFGR &= (uint32_t)0x88FFC00C;

  /*!< Reset HSION, HSEON, CSSON and PLLON bits */
  //RCC->CR &= (uint32_t)0xEEFEFFFE;

  /*!< Reset HSEBYP bit */
  //RCC->CR &= (uint32_t)0xFFFBFFFF;

  /*!< Reset PLLSRC, PLLMUL[3:0] and PLLDIV[1:0] bits */
  //RCC->CFGR &= (uint32_t)0xFF02FFFF;

#elif (SysClockFrequencyHz == 16000000U)

	#if (SysClockExternalHz == 8000000)
  	  	// Setup PLL so we can have 16 MHz using external crystal at 8 MHz

		// Set system clock source to HSE
		// 10: HSE oscillator used as system clock
		{
	  	  const uint32_t wantedClock = 2;
			uint32_t tmpRCC_CFGR = RCC->CFGR;
			tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
			tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
			RCC->CFGR = tmpRCC_CFGR;
		}



		// First turn PLL off and other clocks on.
		RCC->CR |= RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_MSION_Msk;
		RCC->CR &= ~RCC_CR_PLLON_Msk;

		/*
		Below code sets PLL to:
			use HSI (16MHz) as input.
			Multiply by 4
			Divide by 2.
		So result shall be the wanted 32 MHz (if all this works)

		Bits 23:22 PLLDIV[1:0]: PLL output division
			01: PLL clock output = PLLVCO / 2
		Bits 21:18 PLLMUL[3:0]: PLL multiplication factor
			0001: PLLVCO = PLL clock entry x 4
		Bit 16 PLLSRC: PLL entry clock source
			This bit is set and cleared by software to select PLL clock source. This bit can be written
			only when PLL is disabled.
			0: HSI oscillator clock selected as PLL input clock
			1: HSE oscillator clock selected as PLL input clock
			Note: The PLL minimum input clock frequency is 2 MHz.
		*/
		//RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | RCC_CFGR_PLLMUL4 | RCC_CFGR_PLLSRC_HSE;
		RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | (1 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

		// Turn PLL on
		RCC->CR |= RCC_CR_PLLON;

		// TODO Wait for PLL to be stable, for now just busy wait.
		sysBusyWait(10000);


		// Set system clock source to PLL.
		// 11: PLL used as system clock
		uint32_t tmpRCC_CFGR = RCC->CFGR;
		const uint32_t wantedClock = 3; // RCC_CFGR_SW_PLL
		tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
		tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
		RCC->CFGR = tmpRCC_CFGR;

		// Wait for system clock to be on.
		//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
		sysBusyWait(10000);

	#else
		// Use HSI, 16 MHz, no PLL

		// TODO
		/*
		[DocID17659 Rev 12] Table 3
		The CPU frequency changes from initial to final must respect "F CPU initial < 4*F CPU final" to limit V CORE
		drop due to current consumption peak when frequency increases. It must also respect 5 μs delay between
		two changes. For example to switch from 4.2 MHz to 32 MHz, you can switch from 4.2 MHz to 16 MHz,
		wait 5 μs, then switch from 16 MHz to 32 MHz.""
		*/
		// hmm

		// Initialize/Enable the HSI clock and wait until HSI stable
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}


		/* Set HSI as SYSCLCK source */
		// RM0038 Rev 16, 6.3.3 Clock configuration register (RCC_CFGR)
		// 01: HSI oscillator used as system clock
		const uint32_t wantedClock = 1;
		uint32_t tmpRCC_CFGR = RCC->CFGR & ~(RCC_CFGR_SW_Msk);
		RCC->CFGR = tmpRCC_CFGR | (wantedClock << RCC_CFGR_SW_Pos);

		// Wait for system clock to be on.
		//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
		sysBusyWait(10000);

	#endif

	// TODO Turn off MSI to save a little power.
	// RCC->CR &= ~RCC_CR_MSION_Msk;

#elif (SysClockFrequencyHz == 24000000U) || (SysClockFrequencyHz == 32000000U)
  	uint32_t tmp = 0;

	// [RM0038 Rev 16] 5.1.5 "Dynamic voltage scaling management"
	// Table 26. Performance versus V CORE ranges
  	// To use USB device the product must be in range 1.

	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	tmp = PWR->CR & (~PWR_CR_VOS_Msk);
	PWR->CR = tmp | (1 << PWR_CR_VOS_Pos);
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}

	// Bit 28 PWRRST: Power interface reset
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST_Msk;
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}



	// 1: 64-bit access
	// Check that 64-bit access is taken into account by reading FLASH_ACR
	FLASH->ACR |= (1 << FLASH_ACR_ACC64_Pos);
	while ((FLASH->ACR & (1 << FLASH_ACR_ACC64_Pos)) == 0) {sysBusyWait(10);}
	sysBusyWait(1000);

	// Bit 1 PRFTEN: Prefetch enable
	//FLASH->ACR |= (1 << FLASH_ACR_PRFTEN_Pos);
	//sysBusyWait(1000);

    // Set one wait state latency for flash read.
	// RM0038 Rev 16, 3.9.1 Access control register (FLASH_ACR)
	// 1: one wait state
	// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
	// For now just busy wait
	FLASH->ACR |= (1 << FLASH_ACR_LATENCY_Pos);
	while ((FLASH->ACR & (1 << FLASH_ACR_LATENCY_Pos)) == 0) {sysBusyWait(10);}
	sysBusyWait(1000);


	// Then PLL

	// Setup PLL so we can have 24 or 32 MHz
    // [RM0038 Rev 16] 6.2.4 PLL



	/*
	Below code sets PLL to:
		use HSI (16MHz) as input.
		Multiply by 4
		Divide by 2.
	So result shall be the wanted 32 MHz (if all this works)

	Bits 23:22 PLLDIV[1:0]: PLL output division
	 	00: not allowed
		01: PLL clock output = PLLVCO / 2
		10: PLL clock output = PLLVCO / 3
		11: PLL clock output = PLLVCO / 4
	Bits 21:18 PLLMUL[3:0]: PLL multiplication factor
		0000: PLLVCO = PLL clock entry x 3
		0001: PLLVCO = PLL clock entry x 4
		0010: PLLVCO = PLL clock entry x 6
		0011: PLLVCO = PLL clock entry x 8
		0100: PLLVCO = PLL clock entry x 12
		0101: PLLVCO = PLL clock entry x 16
		0110: PLLVCO = PLL clock entry x 24
		0111: PLLVCO = PLL clock entry x 32
		1000: PLLVCO = PLL clock entry x 48
	Bit 16 PLLSRC: PLL entry clock source
		This bit is set and cleared by software to select PLL clock source. This bit can be written
		only when PLL is disabled.
		0: HSI oscillator clock selected as PLL input clock
		1: HSE oscillator clock selected as PLL input clock
		Note: The PLL minimum input clock frequency is 2 MHz.
	*/
	#if SysClockExternalHz == 8000000U
		// Using HSE

		// Set system clock source to HSE, first at 8 MHz
		// 10: HSE oscillator used as system clock
		{
		  const uint32_t wantedClock = 2;
			uint32_t tmpRCC_CFGR = RCC->CFGR;
			tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
			tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
			RCC->CFGR = tmpRCC_CFGR;
		}

		sysBusyWait(1000);

		// First turn PLL off and other clocks on.
		RCC->CR |= RCC_CR_HSEON_Msk | RCC_CR_MSION_Msk;
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		while (RCC->CR & RCC_CR_PLLRDY_Msk) {sysBusyWait(10);}
		sysBusyWait(1000);


		#if SysClockFrequencyHz == 24000000U
			RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | (4 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);
		#elif SysClockFrequencyHz == 32000000U
			RCC->CFGR |= (2 << RCC_CFGR_PLLDIV_Pos) | (4 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);
		#else
		#error
		#endif

	#else
		// Using HSI

		// Initialize/Enable the HSI clock and wait until HSI stable
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(10);
		}

		/*
		[DocID17659 Rev 12] Table 3
		The CPU frequency changes from initial to final must respect "F CPU initial < 4*F CPU final" to limit V CORE
		drop due to current consumption peak when frequency increases. It must also respect 5 μs delay between
		two changes. For example to switch from 4.2 MHz to 32 MHz, you can switch from 4.2 MHz to 16 MHz,
		wait 5 μs, then switch from 16 MHz to 32 MHz.""
		*/
		// hmm

		/* Set HSI as SYSCLCK source */
		// RM0038 Rev 16, 6.3.3 Clock configuration register (RCC_CFGR)
		// 01: HSI oscillator used as system clock
		{
			const uint32_t wantedClock = 1;
			uint32_t tmpRCC_CFGR = RCC->CFGR & ~(RCC_CFGR_SW_Msk);
			RCC->CFGR = tmpRCC_CFGR | (wantedClock << RCC_CFGR_SW_Pos);
		}

		sysBusyWait(1000);

		#if SysClockFrequencyHz == 24000000U
			RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | ( 2 << RCC_CFGR_PLLMUL_Pos);
		#elif SysClockFrequencyHz == 32000000U
			RCC->CFGR |= (2 << RCC_CFGR_PLLDIV_Pos) | ( 2 << RCC_CFGR_PLLMUL_Pos);
		#else
		#error
		#endif

	#endif

	sysBusyWait(1000);

	// Turn PLL on
	RCC->CR |= RCC_CR_PLLON;
	//while (!(RCC->CR & RCC_CR_PLLRDY_Msk)) {sysBusyWait(10);}
	while (RCC->CR & RCC_CR_PLLRDY_Msk) {sysBusyWait(10);}
	sysBusyWait(1000);

	// Set system clock source to PLL.
	// 11: PLL used as system clock
	{
		uint32_t tmpRCC_CFGR = RCC->CFGR;
		const uint32_t wantedClock = 3; // RCC_CFGR_SW_PLL
		tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
		tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
		RCC->CFGR = tmpRCC_CFGR;
	}

	// Wait for system clock to be on.
	//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
	sysBusyWait(100000);

	// Bit 28 PWRRST: Power interface reset
	/*while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST_Msk;
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}*/

	sysBusyWait(100000);

	// TODO Disable unused clocks.
	//RCC->CR &= ~(RCC_CR_MSION_Msk | RCC_CR_HSION_Msk | RCC_CR_HSEON_Msk);

#else
#error
#endif

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;




#ifdef DATA_IN_ExtSRAM
#error not implemented
#endif
    
  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#endif

#if 0
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
#endif
}

