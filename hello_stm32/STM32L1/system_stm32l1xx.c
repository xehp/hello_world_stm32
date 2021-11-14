/**
  ******************************************************************************
  * @file    system_stm32l1xx.c
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    01-July-2016
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  *             
  *   This file provides two functions and one global variable to be called from 
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32l1xx.s" file.
  *                        
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.   
  *      
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32l1xx_system
  * @{
  */  
  
/** @addtogroup STM32L1xx_System_Private_Includes
  * @{
  */

#include "stm32l1xx.h"
#include "cfg.h"
#include "sys.h"


/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Default value of the External oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)8000000) /*!< Default value of the Internal oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSI_VALUE */

/*!< Uncomment the following line if you need to use external SRAM mounted
     on STM32L152D_EVAL board as data memory  */
/* #define DATA_IN_ExtSRAM */
  
/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */ 
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. 
                                  This value must be a multiple of 0x200. */
/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_Variables
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
#if 0
uint32_t SystemCoreClock    = 32000000;
const uint8_t PLLMulTable[9]    = {3, 4, 6, 8, 12, 16, 24, 32, 48};
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};
#endif

/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_FunctionPrototypes
  * @{
  */

#if defined (STM32L151xD) || defined (STM32L152xD) || defined (STM32L162xD)
#ifdef DATA_IN_ExtSRAM
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */
#endif /* STM32L151xD || STM32L152xD || STM32L162xD */

/**
  * @}
  */

/** @addtogroup STM32L1xx_System_Private_Functions
  * @{
  */



/**
  * @brief  Setup the microcontroller system.
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable (if needed).
  * @param  None
  * @retval None
  */
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

	// Changing latency is probably wrong here because must use also FLASH_ACR_ACC64
	// Set one wait state latency for flash read.
	// RM0038 Rev 16, 3.9.1 Access control register (FLASH_ACR)
	// 1: one wait state
	//FLASH->ACR |= (1 << FLASH_ACR_LATENCY_Pos);
	// [1] 3.3.3 Read access latency step 2. Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register.
	// For now just busy wait
	//sysBusyWait(10000);


	#if (SysClockExternalHz == 8000000)

		// Setup PLL so we can have 16 MHz using external crystal at 8 MHz


		// Initialize/Enable the HSI clock and wait until HSI stable
		//RCC->CR |= RCC_CR_HSION_Msk;
		/*while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		{
			sysBusyWait(1);
		}*/
		//sysBusyWait(10000);


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

  	/*
    TODO Not allowed to change clock more than a factor 4?
  	[DocID17659 Rev 12] Table 3
  	The CPU frequency changes from initial to final must respect "F CPU initial < 4*F CPU final" to limit V CORE
  	drop due to current consumption peak when frequency increases. It must also respect 5 μs delay between
  	two changes. For example to switch from 4.2 MHz to 32 MHz, you can switch from 4.2 MHz to 16 MHz,
  	wait 5 μs, then switch from 16 MHz to 32 MHz.""
  	*/


	// Setup PLL so we can have 24 or 32 MHz
    // [RM0038 Rev 16] 6.2.4 PLL

	// [RM0038 Rev 16] 5.1.5 "Dynamic voltage scaling management" perhaps.
	// Table 26. Performance versus V CORE ranges
  	// To use USB device the product must be in range 1.
	/*while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	tmp = PWR->CR & (~PWR_CR_VOS_Msk);
	PWR->CR = tmp | (2 << PWR_CR_VOS_Pos);
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}*/

	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	tmp = PWR->CR & (~PWR_CR_VOS_Msk);
	PWR->CR = tmp | (1 << PWR_CR_VOS_Pos);
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}

	// Bit 28 PWRRST: Power interface reset
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST_Msk;
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}


	// 1: 64-bit access
	FLASH->ACR |= (1 << FLASH_ACR_ACC64_Pos);
	while ((FLASH->ACR & (1 << FLASH_ACR_ACC64_Pos)) == 0) {sysBusyWait(10);}
	sysBusyWait(1000);

	// Check that 64-bit access is taken into account by reading FLASH_ACR


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

		// First turn PLL off and other clocks on.
		RCC->CR |= RCC_CR_HSEON_Msk | RCC_CR_MSION_Msk;
		RCC->CR &= ~RCC_CR_PLLON_Msk;
		while (RCC->CR & RCC_CR_PLLRDY_Msk) {sysBusyWait(10);}
		sysBusyWait(1000);


		#if SysClockFrequencyHz == 24000000U

			// This may enough to run USB device
			RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | (4 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

		#elif SysClockFrequencyHz == 32000000U
			// This line gives only 6 MHz but at least it runs. So baudrate is 21600 instead of 115200
			//RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | (0 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This line gives only 8 MHz but at least it runs. So baudrate is 28800 instead of 115200
			//RCC->CFGR |= (2 << RCC_CFGR_PLLDIV_Pos) | (0 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This line gives only 16 MHz but at least it runs, baudrate 57600
			//RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | (1 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This give 24 MHz, so baudrate is 86400.
			//RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | (2 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This should give the wanted 32 MHz but it does not run.
			//RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | (3 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This line gives only 16 MHz but at least it runs.
			//RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | (3 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// This should also give 48 MHz but it does not run.
			//RCC->CFGR |= (1 << RCC_CFGR_PLLDIV_Pos) | (4 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			// Finally this is what we need to run USB device. This is what we want. But this did not work.
			RCC->CFGR |= (2 << RCC_CFGR_PLLDIV_Pos) | (4 << RCC_CFGR_PLLMUL_Pos) | (1<<RCC_CFGR_PLLSRC_Pos);

			#warning // Failed to get 32 MHz working with external crystal

		#else
		#error
		#endif

	#else
		// Using HSI

		// Initialize/Enable the HSI clock and wait until HSI stable
		RCC->CR |= RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk)) {sysBusyWait(10);}
		sysBusyWait(1000);

		#if SysClockFrequencyHz == 24000000U
			// This may enough to run USB device
			RCC->CFGR |= (3 << RCC_CFGR_PLLDIV_Pos) | ( 2 << RCC_CFGR_PLLMUL_Pos);
		#elif SysClockFrequencyHz == 32000000U
			// This configuration works but sometimes it need more than one try to get it running at power on?
			// This is what we need to run USB device. PLLVCO must be running at 96 MHz.
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
	uint32_t tmpRCC_CFGR = RCC->CFGR;
	const uint32_t wantedClock = 3; // RCC_CFGR_SW_PLL
	tmpRCC_CFGR &= ~(RCC_CFGR_SW_Msk);
	tmpRCC_CFGR |= (wantedClock << RCC_CFGR_SW_Pos);
	RCC->CFGR = tmpRCC_CFGR;

	// Wait for system clock to be on.
	//while (((RCC->CFGR & (3 << RCC_CFGR_SWS_Pos)) != (wantedClock << RCC_CFGR_SW_Pos) ) {};
	sysBusyWait(100000);

	// Bit 28 PWRRST: Power interface reset
	/*while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}
	RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST_Msk;
	while (PWR->CSR & PWR_CSR_VOSF_Msk) {sysBusyWait(10);}*/

	sysBusyWait(100000);

	// TODO Disable unused clocks.
	//RCC->CR &= ~(RCC_CR_MSION_Msk | RCC_CR_HSEON_Msk);


#else
#error
#endif

  /*!< Disable all interrupts */
  RCC->CIR = 0x00000000;




#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM */
    
  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#endif /* USER_VECT_TAB_ADDRESS */

#if 0
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
#endif
}



#if 0
/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is MSI, SystemCoreClock will contain the MSI 
  *             value as defined by the MSI range.
  *                                   
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32l1xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32l1xx.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmul = 0, plldiv = 0, pllsource = 0, msirange = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  
  switch (tmp)
  {
    case 0x00:  /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
    case 0x04:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x08:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x0C:  /* PLL used as system clock */
      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmul = RCC->CFGR & RCC_CFGR_PLLMUL;
      plldiv = RCC->CFGR & RCC_CFGR_PLLDIV;
      pllmul = PLLMulTable[(pllmul >> 18)];
      plldiv = (plldiv >> 22) + 1;
      
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock selected as PLL clock entry */
        SystemCoreClock = (((HSI_VALUE) * pllmul) / plldiv);
      }
      else
      {
        /* HSE selected as PLL clock entry */
        SystemCoreClock = (((HSE_VALUE) * pllmul) / plldiv);
      }
      break;
    default: /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
  }
  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
}

#if defined (STM32L151xD) || defined (STM32L152xD) || defined (STM32L162xD)
#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller.
  *         Called in SystemInit() function before jump to main.
  *         This function configures the external SRAM mounted on STM32L152D_EVAL board
  *         This SRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 +                       SRAM pins assignment                                   +
 +-------------------+--------------------+------------------+------------------+
 | PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0 <-> FSMC_A10 |
 | PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1 <-> FSMC_A11 |
 | PD4  <-> FSMC_NOE | PE7  <-> FSMC_D4   | PF2  <-> FSMC_A2 | PG2 <-> FSMC_A12 |
 | PD5  <-> FSMC_NWE | PE8  <-> FSMC_D5   | PF3  <-> FSMC_A3 | PG3 <-> FSMC_A13 |
 | PD8  <-> FSMC_D13 | PE9  <-> FSMC_D6   | PF4  <-> FSMC_A4 | PG4 <-> FSMC_A14 |
 | PD9  <-> FSMC_D14 | PE10 <-> FSMC_D7   | PF5  <-> FSMC_A5 | PG5 <-> FSMC_A15 |
 | PD10 <-> FSMC_D15 | PE11 <-> FSMC_D8   | PF12 <-> FSMC_A6 | PG10<-> FSMC_NE2 |
 | PD11 <-> FSMC_A16 | PE12 <-> FSMC_D9   | PF13 <-> FSMC_A7 |------------------+
 | PD12 <-> FSMC_A17 | PE13 <-> FSMC_D10  | PF14 <-> FSMC_A8 | 
 | PD13 <-> FSMC_A18 | PE14 <-> FSMC_D11  | PF15 <-> FSMC_A9 | 
 | PD14 <-> FSMC_D0  | PE15 <-> FSMC_D12  |------------------+
 | PD15 <-> FSMC_D1  |--------------------+ 
 +-------------------+
*/

  /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHBENR   = 0x000080D8;
  
  /* Connect PDx pins to FSMC Alternate function */
  GPIOD->AFR[0]  = 0x00CC00CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAA0A0A;
  /* Configure PDx pins speed to 40 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0F0F;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x00000000;

  /* Connect PEx pins to FSMC Alternate function */
  GPIOE->AFR[0]  = 0xC00000CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA800A;
  /* Configure PEx pins speed to 40 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC00F;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x00000000;

  /* Connect PFx pins to FSMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA000AAA;
  /* Configure PFx pins speed to 40 MHz */ 
  GPIOF->OSPEEDR = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x00000000;

  /* Connect PGx pins to FSMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0x00000C00;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0x00200AAA;
  /* Configure PGx pins speed to 40 MHz */ 
  GPIOG->OSPEEDR = 0x00300FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x00000000;
  
/*-- FSMC Configuration ------------------------------------------------------*/
  /* Enable the FSMC interface clock */
  RCC->AHBENR    = 0x400080D8;

  /* Configure and enable Bank1_SRAM3 */
  FSMC_Bank1->BTCR[4]  = 0x00001011;
  FSMC_Bank1->BTCR[5]  = 0x00000300;
  FSMC_Bank1E->BWTR[4] = 0x0FFFFFFF;
/*
  Bank1_SRAM3 is configured as follow:

  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 3;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
*/
  
}
#endif /* DATA_IN_ExtSRAM */
#endif /* STM32L151xD || STM32L152xD || STM32L162xD */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
