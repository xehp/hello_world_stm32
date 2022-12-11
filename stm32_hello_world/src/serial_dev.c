/*
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

Barebone blinky on nucleo L432KC / stm32l432kc using SysTick. No RTOS needed.

NOTE
Trying to support many more chips, not everything is tested after every change.

References
[1] DocID027295 Rev 3, http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/b0/ac/3e/8f/6d/21/47/af/DM00151940/files/DM00151940.pdf/jcr:content/translations/en.DM00151940.pdf
[2] Device Summary STM32L431xx DS11453 Rev 3 (stm32l431kc.pdf)
[3] Reference Manual RM0394 Rev 4 (en.DM00151940.pdf)
[4] RM0038 Rev 16
[5] DocID024330 Rev 5

Ref [4] found at:
https://www.st.com/en/microcontrollers-microprocessors/stm32l151-152.html#documentation
*/


#include "dev_cfg.h"
#include "sys_dev.h"
#include "utility_functions.h"
#include "fifo.h"
#include "serial_dev.h"

#if (defined USART1_BAUDRATE) || (defined USART2_BAUDRATE) || (defined USART3_BAUDRATE)



#if (defined USART1_BAUDRATE)
volatile struct Fifo usart1In = {0,0,{0}};
volatile struct Fifo usart1Out = {0,0,{0}};
#endif

#ifdef USART2_BAUDRATE

	// Do we support sending on Usart2. Comment this out if not needed.
    // Typical transmit pin is PA2, but PD5 may be possible to use on some devices.
    // USART2_TX_PIN can be set to 2 or 5 here for PA2 or PD5 respectively.
	#define USART2_TX_PIN 2

	// On which pin shall Usart2 transmit, use 3 or 15.
	// On Nucleo PA3 does not work via stlink (PA2 does work for sending), use PA_15 for usart2 RX instead.
	// https://community.st.com/s/question/0D50X00009XkYKK/usart-vcp-connections-on-nucleol432kc
    // USART2_RX_PIN can be set to 3, 15 or 6 here for PA3, PA15 or PD6 respectively.
	#if (defined STM32L432xx)
	#define USART2_RX_PIN 15
	#else
	#define USART2_RX_PIN 3
	#endif

	#ifdef USART2_RX_PIN
	volatile struct Fifo usart2In = {0,0,{0}};
	#endif

	#ifdef USART2_TX_PIN
	volatile struct Fifo usart2Out = {0,0,{0}};
	#endif
#endif

#ifdef USART3_BAUDRATE
	volatile struct Fifo usart3In = {0,0,{0}};
	volatile struct Fifo usart3Out = {0,0,{0}};
#endif


#if (defined STM32L432xx) || (defined STM32F303x8)

	// NOTE Two uarts, usarts etc shall not use same pins.
	// For example USART2 and LPUART1 can not both use PA2.
	#if ((defined LPUART1_TX_PIN) && (defined USART2_TX_PIN)) && (LPUART1_TX_PIN==USART2_TX_PIN)
	#error
	#endif


	#ifdef LPUART1_BAUDRATE
	void __attribute__ ((interrupt, used)) LPUART1_IRQHandler(void)
	{
	  volatile uint32_t tmp = LPUART1->ISR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
	    // Simple receive, this never happens :(
	    //mainDummy = USART1->RDR;
	    fifoPut(&lpuart1In, LPUART1->RDR);
	  }

	  #ifdef LPUART1_TX_PIN
	  // TXE (transmit empty)
	  if (tmp & USART_ISR_TXE_Msk)
	  {
	    if (!fifoIsEmpty(&lpuart1Out))
	    {
	      LPUART1->TDR = fifoTake(&lpuart1Out);
	    }
	    else
	    {
	      // We are done sending, disables further tx empty interrupts.
	      LPUART1->CR1 &= ~(USART_CR1_TXEIE_Msk);
	    }
	  }
	  #endif
	}
	#endif


	#ifdef USART1_BAUDRATE
	void __attribute__ ((interrupt, used)) USART1_IRQHandler(void)
	{
	  const uint32_t tmp = USART1->ISR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
		fifoPut(&usart1In, USART1->RDR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_ISR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart1Out))
		{
		  USART1->TDR = fifoTake(&usart1Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART1->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif

	#ifdef USART2_BAUDRATE
	void __attribute__ ((interrupt, used)) USART2_IRQHandler(void)
	{
	  const uint32_t tmp = USART2->ISR;

	  #ifdef USART2_RX_PIN
	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
		fifoPut(&usart2In, (int)(USART2->RDR & 0xFF));
	  }
	  #endif

	  #ifdef USART2_TX_PIN
	  // TXE (transmit empty)
	  if (tmp & USART_ISR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart2Out))
		{
		  USART2->TDR = fifoTake(&usart2Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART2->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	  #endif
	}
	#endif


	#ifdef USART3_BAUDRATE
	void __attribute__ ((interrupt, used)) USART3_IRQHandler(void)
	{
	  volatile uint32_t tmp = USART3->ISR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
		fifoPut(&usart3In, USART3->RDR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_ISR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart3Out))
		{
		  USART3->TDR = fifoTake(&usart3Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART3->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif


#elif (defined STM32L151xB)

	#ifdef USART1_BAUDRATE
	void __attribute__ ((interrupt, used)) USART1_IRQHandler(void)
	{
	  volatile uint32_t tmp = USART1->SR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		fifoPut(&usart1In, USART1->DR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_SR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart1Out))
		{
		  USART1->DR = fifoTake(&usart1Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART1->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif

	#ifdef USART2_BAUDRATE
	void __attribute__ ((interrupt, used)) USART2_IRQHandler(void)
	{
	  const uint32_t tmp = USART2->SR;

	  #ifdef USART2_RX_PIN
	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		fifoPut(&usart2In, (int)(USART2->DR & 0xFF));
	  }
	  #endif

	  #ifdef USART2_TX_PIN
	  // TXE (transmit empty)
	  if (tmp & USART_SR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart2Out))
		{
		  USART2->DR = fifoTake(&usart2Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART2->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	  #endif
	}
	#endif

	#ifdef USART3_BAUDRATE
	void __attribute__ ((interrupt, used)) USART3_IRQHandler(void)
	{
	  const uint32_t tmp = USART3->SR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		fifoPut(&usart3In, (int)(USART3->DR & 0xFF));
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_SR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart3Out))
		{
		  USART3->DR = fifoTake(&usart3Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART3->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif

#elif (defined STM32F030x8)

	#ifdef USART1_BAUDRATE
	void __attribute__ ((interrupt, used)) USART1_IRQHandler(void)
	{
		const uint32_t tmp = USART1->ISR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
		fifoPut(&usart1In, USART1->RDR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_ISR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart1Out))
		{
		  USART1->TDR = fifoTake(&usart1Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART1->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif

	#ifdef USART2_BAUDRATE
	void __attribute__ ((interrupt, used)) USART2_IRQHandler(void)
	{
		//SYS_LED_ON();

		const uint32_t tmp = USART2->ISR;

		#ifdef USART2_RX_PIN
		// RXNE (Receive not empty)
		if (tmp & USART_ISR_RXNE_Msk)
		{
			fifoPut(&usart2In, (int)(USART2->RDR & 0xFF));
		}
		#endif

		#ifdef USART2_TX_PIN
		// TXE (transmit empty)
		if (tmp & USART_ISR_TXE_Msk)
		{
			if (!fifoIsEmpty(&usart2Out))
			{
				USART2->TDR = fifoTake(&usart2Out);
			}
			else
			{
				// We are done sending, disables further tx empty interrupts.
				USART2->CR1 &= ~(USART_CR1_TXEIE_Msk);
			}
		}
		#endif

		//SYS_LED_OFF();

		// This line is probably not needed.
		USART2->ICR = 0x2A5F;

	}
	#endif

	#ifdef USART3_BAUDRATE
	#error not implemented yet
	#endif

#elif (defined STM32F103xB)

	#ifdef USART1_BAUDRATE
	void __attribute__ ((interrupt, used)) USART1_IRQHandler(void)
	{
	  const uint32_t tmp = USART1->SR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		fifoPut(&usart1In, USART1->DR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_SR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart1Out))
		{
		  USART1->DR = fifoTake(&usart1Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART1->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif
	#ifdef USART2_BAUDRATE
	void __attribute__ ((interrupt, used)) USART2_IRQHandler(void)
	{
		const uint32_t tmp = USART2->SR;

		#ifdef USART2_RX_PIN
		// RXNE (Receive not empty)
		if (tmp & USART_SR_RXNE_Msk)
		{
			fifoPut(&usart2In, (int)(USART2->DR & 0xFF));
		}
		#endif

		#ifdef USART2_TX_PIN
		// TXE (transmit empty)
		if (tmp & USART_SR_TXE_Msk)
		{
			if (!fifoIsEmpty(&usart2Out))
			{
				USART2->DR = fifoTake(&usart2Out);
			}
			else
			{
				// We are done sending, disables further tx empty interrupts.
				USART2->CR1 &= ~(USART_CR1_TXEIE_Msk);
			}
		}
		#endif
	}
	#endif

	#ifdef USART3_BAUDRATE
	void __attribute__ ((interrupt, used)) USART3_IRQHandler(void)
	{
	  const uint32_t tmp = USART3->SR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		fifoPut(&usart3In, USART3->DR);
	  }

	  // TXE (transmit empty)
	  if (tmp & USART_SR_TXE_Msk)
	  {
		if (!fifoIsEmpty(&usart3Out))
		{
		  USART3->DR = fifoTake(&usart3Out);
		}
		else
		{
		  // We are done sending, disables further tx empty interrupts.
		  USART3->CR1 &= ~(USART_CR1_TXEIE_Msk);
		}
	  }
	}
	#endif

#else
#error
#endif

#ifndef STM32F103xB

void setupIoPinTx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction)
{
    uint32_t tmp = 0x00;

    /*
    Alternate function register.
    Set it to USART2. Remember to set the port mode to alternate mode also.
    */
    const uint32_t afrIndex = pin >> 3;
    const uint32_t afrOffset = (pin & 0x7) * 4;
    tmp = base->AFR[afrIndex];
    tmp &= ~(15U << afrOffset);
    tmp |= alternateFunction << afrOffset;
    base->AFR[afrIndex] = tmp;

    /* [1]
    Configure IO Direction mode (Input, Output, Alternate or Analog)
    00: Input mode
    01: General purpose output mode
    10: Alternate function mode
    11: Analog mode (reset state)
    Need the alternate mode in this case. See also AFR.
    */
    const uint32_t mode = 2U; // Alternate function mode
    tmp = base->MODER;
    tmp &= ~(3U << (pin * 2));
    tmp |= (mode << (pin * 2));
    base->MODER = tmp;

    /* [1]
    Configure the IO Speed
    00: Low speed (reset state for most IO pins)
    01: Medium speed
    10: High speed
    11: Very high speed (reset state for PA13)
    */
    const uint32_t speed = 2U; // High speed
    tmp = base->OSPEEDR;
    tmp &= ~(3U << (pin * 2));
    tmp |= (speed << (pin * 2));
    base->OSPEEDR = tmp;
}

#else

void setupIoPinTx(GPIO_TypeDef *base, uint32_t pin)
{
	const uint32_t cnf_mode = 0x9; // Alternate Function output, Push-pull, Maximum output speed 10 MHz
	if (pin >= 8)
	{
		const int pos = (pin-8)*4;
		uint32_t tmp = base->CRH;
		tmp &= ~(0xFU << (pos)); // Clear the 4 bits for this pin.
		tmp |= (cnf_mode << (pos)); // Set the 4 bits for this pin.
		base->CRH = tmp;
	}
	else
	{
		const int pos = (pin)*4;
		uint32_t tmp = base->CRL;
		tmp &= ~(0xFU << (pos)); // Clear the 4 bits for this pin.
		tmp |= (cnf_mode << (pos)); // Set the 4 bits for this pin.
		base->CRL = tmp;
	}
}

#endif

/**
 * Perhaps both TX and RX pins can be configured the same way.
 * If so only one method setupIoPinRxTx should be needed.
 * */

#ifndef STM32F103xB

void setupIoPinRx(GPIO_TypeDef *base, uint32_t pin, uint32_t alternateFunction)
{
    uint32_t tmp = 0x00;

    /*
    Alternate function register.
    Set it to USART2. Remember to set the port mode to alternate mode also.
    */
    const uint32_t afrIndex = pin >> 3;
    const uint32_t afrOffset = (pin & 0x7) * 4;
    tmp = base->AFR[afrIndex];
    tmp &= ~(15U << afrOffset);
    tmp |= alternateFunction << afrOffset;
    base->AFR[afrIndex] = tmp;

    /*
    Configure IO Direction mode (Input, Output, Alternate or Analog)
    00: Input mode
    01: General purpose output mode
    10: Alternate function mode
    11: Analog mode (reset state)
    Need the alternate mode in this case. See also AFR.
    I think it shall be alternate mode but perhaps it shall be input mode?
    */
    const uint32_t mode = 2U;
    tmp = base->MODER;
    tmp &= ~(3U << (pin * 2));
    tmp |= (mode << (pin * 2));
    base->MODER = tmp;

    /*
    Configure the IO Speed
    00: Low speed (reset state for most IO pins)
    01: Medium speed
    10: High speed
    11: Very high speed (reset state for PA13)
    */
    const uint32_t speed = 2U;
    tmp = base->OSPEEDR;
    tmp &= ~(3U << (pin * 2));
    tmp |= (speed << (pin * 2));
    base->OSPEEDR = tmp;


    /*
    Activate the Pull-up or Pull down resistor for the current IO
    00: No pull-up, pull-down (reset state for most IO pins)
    01: Pull-up
    10: Pull-down
    11: Reserved
    Seems resonable to activate Pull-up if open drain is used, but did not notice any improvement doing so.
    */
    const uint32_t pupd = 1U;
    tmp = GPIOA->PUPDR;
    tmp &= ~(3U << (pin * 2));
    tmp |= (pupd << (pin * 2));
    base->PUPDR = tmp;
}

#else

void setupIoPinRx(GPIO_TypeDef *base, uint32_t pin)
{
	// Hoping this is this how to set it for pull up.
	// Table 20. Port bit configuration table in RM0008 Rev 21.
	const uint32_t cnf_mode = 0x8; // Want Input pull up.
	if (pin >= 8)
	{
		const int pos = (pin-8)*4;
		uint32_t tmp = base->CRH;
		tmp &= ~(0xFU << (pos)); // Clear the 4 bits for this pin.
		tmp |= (cnf_mode << (pos)); // Set the 4 bits for this pin.
		base->CRH = tmp;
	}
	else
	{
		const int pos = (pin)*4;
		uint32_t tmp = base->CRL;
		tmp &= ~(0xFU << (pos)); // Clear the 4 bits for this pin.
		tmp |= (cnf_mode << (pos)); // Set the 4 bits for this pin.
		base->CRL = tmp;
	}
	base->ODR |= 1 << pin;
}

#endif


#ifdef LPUART1_BAUDRATE
int lpuartInit(uint32_t baud)
{

  /* Disable the Peripheral, should be the reset state but just in case. */
  LPUART1->CR1 &= ~USART_CR1_UE_Msk;

  /*
  USART1_TX on PA9, USART1_RX on PA10.
  But it is Usart2 that is connected to USB so using that one.
  USART2 GPIO Configuration: USART2_TX on PA2, USART2_RX on PA3
  */

  // Clear the in and out FIFOs
  fifoInit(&lpuart1In);
  #ifdef LPUART1_TX_PIN
  fifoInit(&lpuart1Out);
  #endif

  // Enable Uart clock LPUART1EN
  RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN_Msk;

  #if LPUART1_BAUDRATE == 9600
  // Select clock LPUART1SEL
  {
	// [3] 6.4.27 Peripherals independent clock configuration register (RCC_CCIPR)
	// Bits 11:10 LPUART1SEL[1:0]: LPUART1 clock source selection
	// These bits are set and cleared by software to select the LPUART1 clock source.
	// 00: PCLK selected as LPUART1 clock
	// 01: System clock (SYSCLK) selected as LPUART1 clock
	// 10: HSI16 clock selected as LPUART1 clock
	// 11: LSE clock selected as LPUART1 clock
    uint32_t tmp = RCC->CCIPR;
    uint32_t sel = 2u;
    tmp &= ~(3 << RCC_CCIPR_LPUART1SEL_Pos);
    tmp |= (sel << RCC_CCIPR_LPUART1SEL_Pos);
    RCC->CCIPR = tmp;
  }
  #endif


  // On Nucleo L432KC we use Serial2 on PA2 & PA15
  // So PA3 is free to use for LPUART1 but we have no transmit pin available.
  // TODO Use LPUART1 for receiving and SoftUart1 for transmitting.

  // Use PA2 for TX & PA3 for RX
  #ifdef LPUART1_TX_PIN
  setupIoPinTx(GPIOA, LPUART1_TX_PIN, GPIO_AF8_LPUART1);
  #endif
  #ifdef LPUART1_RX_PIN
  setupIoPinRx(GPIOA, LPUART1_RX_PIN, GPIO_AF8_LPUART1);
  #endif



  /*
  Select the desired baud rate using the USART_BRR register.
  https://community.st.com/thread/46664-setting-the-baudrate-for-usart-in-stm32f103rb
  Ref [1] chapter 36.5.4 USART baud rate generation
  */
  #if LPUART1_BAUDRATE == 115200
  uint32_t clockDiv = 0x2B671;  // according to Table 203 in ref [3].
  #elif LPUART1_BAUDRATE == 38400
  uint32_t clockDiv = 0x82355;
  #elif LPUART1_BAUDRATE == 9600
  //uint32_t clockDiv = 0x369;  // according to Table 202 in ref [3].
  uint32_t clockDiv = 426667; // (256*16000000LL)/9600;
  #else
  #error
  #endif

  // LPUART1->BRR is a 20 bit register, no use setting larger than 0xfffff.
  // Less than 0x300 is not allowed according to [3] 39.4.4 LPUART baud rate generation
  if ((clockDiv<0x300) || (clockDiv>0xfffff))
  {
    return -1;
  }

  LPUART1->BRR = clockDiv;

  /* Enable uart transmitter and receiver */
  LPUART1->CR1 |= USART_CR1_TE_Msk | USART_CR1_RE_Msk;


  // Bit 1 UESM: USART enable in Stop mode
  // just testing, this should not be needed in this example.
  //LPUART1->CR1 |= USART_CR1_UESM_Msk;

  /*
  In asynchronous mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
  */
  //LPUART1->CR2 &= ~(USART_CR2_LINEN_Msk | USART_CR2_CLKEN_Msk);
  //LPUART1->CR3 &= ~(USART_CR3_SCEN_Msk | USART_CR3_HDSEL_Msk | USART_CR3_IREN_Msk);
  LPUART1->CR2 &= ~(USART_CR2_CLKEN_Msk);



  //Set Usart1 interrupt priority. Lower number is higher priority.
  //uint32_t prioritygroup = NVIC_GetPriorityGrouping();
  //NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(prioritygroup, 45, 0));
  const int irqN = usartGetIrqN(DEV_LPUART1);
  NVIC_SetPriority (irqN, (1UL << __NVIC_PRIO_BITS) - 1UL);

  // This one helped.
  // https://community.st.com/thread/19735
  // I was missing to do NVIC_EnableIRQ.
  NVIC_EnableIRQ(irqN);

  /* Enable the Peripheral */
  LPUART1->CR1 |= USART_CR1_UE_Msk;

  /*
  Shall interrupt be enabled before or after usart is enabled?
  Did not notice any difference doing this before or after.
  RXNEIE
  0: Interrupt is inhibited
  1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_ISR
  */
  LPUART1->CR1 |= USART_CR1_RXNEIE_Msk;

  return 0;
}
#endif

#ifdef LPUART1_TX_PIN
static inline void lpuart1Put(int ch)
{
	fifoPut(&lpuart1Out, ch);
	LPUART1->CR1 |= USART_CR1_TXEIE_Msk;
}
#endif

#ifdef LPUART1_TX_PIN
static inline void usartWriteLpuart1(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		lpuart1Put(*str++);
	    msgLen--;
	}
}
#endif



#if (defined USART1_BAUDRATE)

void serial1PutChar(int ch)
{
	fifoPut(&usart1Out, ch);

	/*
	Now we need to trigger the ISR. Its done by enabling transmitter empty interrupt (it is empty so).
	TXEIE
	0: Interrupt is inhibited
	1: A USART interrupt is generated whenever TXE=1 in the USART_ISR register
	*/
	//USART1->ISR |= USART_ISR_TXE_Msk;
	USART1->CR1 |= USART_CR1_TXEIE_Msk;
}

void serial1Write(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		serial1PutChar(*str++);
	    msgLen--;
	}
}

void serial1Print(const char *str)
{
	while(*str)
	{
		serial1PutChar(*str++);
	}
}

int serial1GetChar()
{
	if (!fifoIsEmpty(&usart1In))
	{
		return fifoTake(&usart1In);
	}
	return -1;
}

/**
This sets up the Usart 1.

USART1_TX on PA9, USART1_RX on PA10.

Returns 0 if OK.
*/
int serial1Init()
{
	/* Disable the Peripheral, should be the reset state but just in case. */
	USART1->CR1 &= ~USART_CR1_UE_Msk;

	// Clear the in and out FIFOs
	fifoInit(&usart1In);
	fifoInit(&usart1Out);

	// For STM32F103xB we used MX Cube SW to setup USART pins, so don't to it again here.
	#if (!defined STM32F103xB) && (!defined STM32F030x8)

	// Enable Usart1 clock & device
	#if (defined STM32L432xx)
	//RCC->APB2ENR1 |= RCC_APB2ENR1_USART1EN_Msk;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN_Msk;
	#elif (defined STM32L151xB) || (defined STM32F303x8) || (defined STM32F030x8) || (defined STM32F103xB)
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN_Msk;
	#else
	#error Unknown device
	#endif

	// Configure IO pins.
	#if (defined STM32F030x8)
	setupIoPinTx(GPIOA, 9, GPIO_AF1_USART1);
	setupIoPinRx(GPIOA, 10, GPIO_AF1_USART1);
	#elif (defined STM32F103xB)
	setupIoPinTx(GPIOA, 9);
	setupIoPinRx(GPIOA, 10);
	#elif 1
	// Using PA9/AF7 & PA10/AF7
	setupIoPinTx(GPIOA, 9, GPIO_AF7_USART1);
	setupIoPinRx(GPIOA, 10, GPIO_AF7_USART1);
	#else
	// See Ref [2] table 16 or Ref [5] table 10
	// If PA9 & PA10 are needed by something else try these instead:
	// Using PB6/AF7 & PB7/AF7
	setupIoPinTx(GPIOB, 6, GPIO_AF7_USART1);
	setupIoPinRx(GPIOB, 7, GPIO_AF7_USART1);
	#endif


	/*
	 [1] chapter 36.8.1 Control register 1 (USART_CR1)
	 36.8.12 USART register map
	 8-bit character length: M[1:0] = 00, that is default.
	 1 stop bit is the default value of number of stop bits.
	 That is what we want. So skipping those settings.
	 */

	/*
	 OVER8
	 0: Oversampling by 16 (default)
	 1: Oversampling by 8
	 */
	//usartPtr->CR1 |= (1 << 15);
	/*
	 Select the desired baud rate using the USART_BRR register.
	 https://community.st.com/thread/46664-setting-the-baudrate-for-usart-in-stm32f103rb
	 Ref [1] chapter 36.5.4 USART baud rate generation
	 */
	USART1->BRR = DIV_ROUND(SysClockFrequencyHz, USART1_BAUDRATE);


	/* Enable uart transmitter and receiver */
	USART1->CR1 |= USART_CR1_TE_Msk | USART_CR1_RE_Msk;

	// Bit 1 UESM: USART enable in Stop mode
	// just testing, this should not be needed in this example.
	//usartPtr->CR1 |= USART_CR1_UESM_Msk;

	// We may need to invert RX line if we use a simple optocoupler
	// That is done in CR2 Bit 16 RXINV like this:
	// usartPtr->CR2 |= USART_CR2_RXINV_Msk;
	// No we may need to invert TX line:
	// usartPtr->CR2 |= USART_CR2_TXINV_Msk;

	/*
	 In asynchronous mode, the following bits must be kept cleared:
	 - LINEN and CLKEN bits in the USART_CR2 register,
	 - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
	 */
	//USART1->CR2 &= ~(USART_CR2_LINEN_Msk | USART_CR2_CLKEN_Msk);
	//USART1->CR3 &= ~(USART_CR3_SCEN_Msk | USART_CR3_HDSEL_Msk | USART_CR3_IREN_Msk);
	USART1->CR2 &= ~(USART_CR2_CLKEN_Msk);

	#else
	// TODO Enable pull up
	#endif

	sysBusyWait(2);

	//Set Usart1 interrupt priority. Lower number is higher priority.
	//uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	//NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(prioritygroup, 45, 0));
	NVIC_SetPriority(USART1_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// This one helped.
	// https://community.st.com/thread/19735
	// I was missing to do NVIC_EnableIRQ.
	NVIC_EnableIRQ(USART1_IRQn);

	/* Enable the Peripheral */
	USART1->CR1 |= USART_CR1_UE_Msk;

	/*
	 Shall interrupt be enabled before or after usart is enabled?
	 Did not notice any difference doing this before or after.
	 RXNEIE
	 0: Interrupt is inhibited
	 1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_ISR
	 */
	USART1->CR1 |= USART_CR1_RXNEIE_Msk;

	return 0;
}

#endif


#ifdef USART2_BAUDRATE

void serial2PutChar(int ch)
{
	// If FIFO is full shall we wait or not?
	// Comment out lines below if not.
	while (fifoIsFull(&usart2Out))
	{
		sysSleep();
	}

	fifoPut(&usart2Out, ch);

	// Now we need to trigger the ISR. Its done by enabling transmitter empty interrupt (it is empty so).
	USART2->CR1 |= USART_CR1_TXEIE_Msk;
}

void serial2Write(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		serial2PutChar(*str++);
	    msgLen--;
	}
}

void serial2Print(const char *str)
{
	while(*str)
	{
		serial2PutChar(*str++);
	}
}

int serial2GetChar()
{
	if (!fifoIsEmpty(&usart2In))
	{
		return fifoTake(&usart2In);
	}
	return -1;
}

/**
This sets up the Usart 2.

Usart2 sends on USB (perhaps also on PA2), it receives on PA3.

Returns 0 if OK.
*/
int serial2Init()
{
	/* Disable the Peripheral, should be the reset state but just in case. */
	USART2->CR1 &= ~USART_CR1_UE_Msk;

	// Init FIFOs
	#ifdef USART2_RX_PIN
	fifoInit(&usart2In);
	#else
	#error
	#endif
	#ifdef USART2_TX_PIN
	fifoInit(&usart2Out);
	#else
	#error
	#endif

	// For STM32F103xB (and some other) we used MX Cube SW to setup USART pins, so don't to it again here.
	#if (!defined STM32F103xB) && (!defined STM32F030x8)

	// Enable Usart2 device clock
	#if (defined STM32L432xx)
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN_Msk;  // bit 17
	#elif (defined STM32L151xB) || (defined STM32F303x8) || (defined STM32F030x8) || (defined STM32F103xB)
	RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk;
	#else
	#error Unknown device
	#endif

	// Setup transmit pin (if there is one)
	#ifdef USART2_TX_PIN
		#if USART2_TX_PIN == 2
			#if (defined STM32F030x8)
			setupIoPinTx(GPIOA, 2, GPIO_AF1_USART2);
			#elif (defined STM32F103xB)
			setupIoPinTx(GPIOA, 2);
			#else
			setupIoPinTx(GPIOA, 2, GPIO_AF7_USART2);
			#endif
		#elif (USART2_TX_PIN == 5) && (defined STM32L432xx)
		// Configure IO pins, to find pins and alternate function codes for STM32L432xx see table 16 in ref [2].
		// For STM32L151xB see table 10 in ref [5] to find which alternate function to use.
		setupIoPinTx(GPIOD, 5, GPIO_AF7_USART2);
		#else
		#error
		#endif
	#endif

	// Setup receive pin (if there is one)
	#ifdef USART2_RX_PIN
		#if USART2_RX_PIN == 3
			#if (defined STM32F0)
			// Want Usart2 RX on PA3 so its AF1
			setupIoPinRx(GPIOA, 3, GPIO_AF1_USART2);
			#elif (defined STM32F103xB)
			setupIoPinRx(GPIOA, 2);
			#else
			// Want Usart2 RX on PA3 so its AF7, ref [2] & [5] give same.
			setupIoPinRx(GPIOA, 3, GPIO_AF7_USART2);
			#endif
		#elif (USART2_RX_PIN == 6) && (defined STM32L432xx)
		// Want Usart2 RX on PD6 so its AF7
		setupIoPinRx(GPIOD, 3, GPIO_AF7_USART2);
		#elif (USART2_RX_PIN == 15) && (defined STM32L432xx)
		// Receive pin (normally it would be PA3 AF7, see ref [2].
		// But on Nucleo its not available so we need to use PA15 AF3.
		// Want Usart2 RX on PA15 so its AF3 (not AF7), see ref [2]. Not possible on STM32L151xB.
		setupIoPinRx(GPIOA, 15, GPIO_AF3_USART2);
		#else
		#error
		#endif
	#endif

	/*
	 [1] chapter 36.8.1 Control register 1 (USART_CR1)
	 36.8.12 USART register map
	 8-bit character length: M[1:0] = 00, that is default.
	 1 stop bit is the default value of number of stop bits.
	 That is what we want. So skipping those settings.
	 */

	/*
	 OVER8
	 0: Oversampling by 16 (default)
	 1: Oversampling by 8
	 */
	//usartPtr->CR1 |= (1 << 15);
	/*
	 Select the desired baud rate using the USART_BRR register.
	 https://community.st.com/thread/46664-setting-the-baudrate-for-usart-in-stm32f103rb
	 Ref [1] chapter 36.5.4 USART baud rate generation
	 */
	USART2->BRR = DIV_ROUND(SysClockFrequencyHz, USART2_BAUDRATE);


	/* Enable uart transmitter and receiver */
	USART2->CR1 |= USART_CR1_TE_Msk | USART_CR1_RE_Msk;

	// Bit 1 UESM: USART enable in Stop mode
	// just testing, this should not be needed in this example.
	//usartPtr->CR1 |= USART_CR1_UESM_Msk;

	// We may need to invert RX line if we use a simple optocoupler
	// That is done in CR2 Bit 16 RXINV like this:
	// usartPtr->CR2 |= USART_CR2_RXINV_Msk;
	// No we may need to invert TX line:
	// usartPtr->CR2 |= USART_CR2_TXINV_Msk;

	/*
	 In asynchronous mode, the following bits must be kept cleared:
	 - LINEN and CLKEN bits in the USART_CR2 register,
	 - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
	 */
	//USART2->CR2 &= ~(USART_CR2_LINEN_Msk | USART_CR2_CLKEN_Msk);
	//USART2->CR3 &= ~(USART_CR3_SCEN_Msk | USART_CR3_HDSEL_Msk | USART_CR3_IREN_Msk);
	USART2->CR2 &= ~(USART_CR2_CLKEN_Msk);

	#else
	// TODO Rx pin may need Pull-up
	#endif

	sysBusyWait(2);

	//Set Usart2 interrupt priority. Lower number is higher priority.
	//uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	//NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(prioritygroup, 45, 0));
	NVIC_SetPriority(USART2_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// This one helped.
	// https://community.st.com/thread/19735
	// I was missing to do NVIC_EnableIRQ.
	NVIC_EnableIRQ(USART2_IRQn);

	/* Enable the Peripheral */
	USART2->CR1 |= USART_CR1_UE_Msk;

	/*
	 Shall interrupt be enabled before or after usart is enabled?
	 Did not notice any difference doing this before or after.
	 RXNEIE
	 0: Interrupt is inhibited
	 1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_ISR
	 */
	USART2->CR1 |= USART_CR1_RXNEIE_Msk;

	return 0;
}

#endif


#ifdef USART3_BAUDRATE

void serial3PutChar(int ch)
{
	// If FIFO is full shall we wait or not?
	// Comment out lines below if not.
	//while (fifoIsFull(usart3Out))
	//{
	//	systemSleep();
	//}

	fifoPut(&usart3Out, ch);

	// Now we need to trigger the ISR. Its done by enabling transmitter empty interrupt (it is empty so).
	USART3->CR1 |= USART_CR1_TXEIE_Msk;
}

void serial3Write(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		serial3PutChar(*str++);
	    msgLen--;
	}
}

void serial3Print(const char *str)
{
	while(*str)
	{
		serial3PutChar(*str++);
	}
}

int serial3GetChar()
{
	if (!fifoIsEmpty(&usart3In))
	{
		return fifoTake(&usart3In);
	}
	return -1;
}

/**
This sets up the Usart 3.

Usart2 sends on USB (perhaps also on PA2), it receives on PA3.

Returns 0 if OK.
*/
int serial3Init()
{
	/* Disable the Peripheral, should be the reset state but just in case. */
	USART3->CR1 &= ~USART_CR1_UE_Msk;

	// Clear the in and out FIFOs
	// Enable Usart clock
	fifoInit(&usart3In);
	fifoInit(&usart3Out);


	// For STM32F103xB we used MX Cube SW to setup USART pins, so don't to it again here.
	#ifndef STM32F103xB

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk;

	// For STM32L151xB see table 10 in ref [5].
	// Want Usart3 on TX=PB10 RX=PB11, so its AF7 for both.
	// It is also possible to use PC4, PC5, PC10, PC11 (all AF7) on some devices.
	// See table 16 in ref [2].

	// Transmit pin PB10/AF7
	setupIoPinTx(GPIOB, 10, GPIO_AF7_USART2);

	// Receive pin PB11/AF7
	setupIoPinRx(GPIOB, 11, GPIO_AF7_USART2);

	/*
	 [1] chapter 36.8.1 Control register 1 (USART_CR1)
	 36.8.12 USART register map
	 8-bit character length: M[1:0] = 00, that is default.
	 1 stop bit is the default value of number of stop bits.
	 That is what we want. So skipping those settings.
	 */

	/*
	 OVER8
	 0: Oversampling by 16 (default)
	 1: Oversampling by 8
	 */
	//usartPtr->CR1 |= (1 << 15);
	/*
	 Select the desired baud rate using the USART_BRR register.
	 https://community.st.com/thread/46664-setting-the-baudrate-for-usart-in-stm32f103rb
	 Ref [1] chapter 36.5.4 USART baud rate generation
	 */
	USART3->BRR = DIV_ROUND(SysClockFrequencyHz, USART3_BAUDRATE);


	/* Enable uart transmitter and receiver */
	USART3->CR1 |= USART_CR1_TE_Msk | USART_CR1_RE_Msk;

	// Bit 1 UESM: USART enable in Stop mode
	// just testing, this should not be needed in this example.
	//usartPtr->CR1 |= USART_CR1_UESM_Msk;

	// We may need to invert RX line if we use a simple optocoupler
	// That is done in CR2 Bit 16 RXINV like this:
	// usartPtr->CR2 |= USART_CR2_RXINV_Msk;
	// No we may need to invert TX line:
	// usartPtr->CR2 |= USART_CR2_TXINV_Msk;

	/*
	 In asynchronous mode, the following bits must be kept cleared:
	 - LINEN and CLKEN bits in the USART_CR2 register,
	 - SCEN, HDSEL and IREN  bits in the USART_CR3 register.
	 */
	//USART2->CR2 &= ~(USART_CR2_LINEN_Msk | USART_CR2_CLKEN_Msk);
	//USART2->CR3 &= ~(USART_CR3_SCEN_Msk | USART_CR3_HDSEL_Msk | USART_CR3_IREN_Msk);
	USART3->CR2 &= ~(USART_CR2_CLKEN_Msk);


	#else

	// Transmit pin PB10 (should be configured already)
	//setupIoPinTx(GPIOB, 10);

	// Receive pin PB11 (activate pull up because MX Cube did not do that).
	setupIoPinRx(GPIOB, 11);

	#endif

	sysBusyWait(2);

	//Set Usart3 interrupt priority. Lower number is higher priority.
	//uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	//NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(prioritygroup, 45, 0));
	NVIC_SetPriority(USART3_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// This one helped.
	// https://community.st.com/thread/19735
	// I was missing to do NVIC_EnableIRQ.
	NVIC_EnableIRQ(USART3_IRQn);

	/* Enable the Peripheral */
	USART3->CR1 |= USART_CR1_UE_Msk;

	/*
	 Shall interrupt be enabled before or after usart is enabled?
	 Did not notice any difference doing this before or after.
	 RXNEIE
	 0: Interrupt is inhibited
	 1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_ISR
	 */
	USART3->CR1 |= USART_CR1_RXNEIE_Msk;

	return 0;
}

#endif



void serialLog(const char *str, int n)
{
	#if CONSOLE_DEV == 1
	serial1Write(str, n);
	#elif CONSOLE_DEV == 2
	serial2Write(str, n);
	#elif CONSOLE_DEV == 3
	serial3Write(str, n);
	#else
	#warning No logging available
	#endif
	sysSleepMs(10);
}


void serialLogLine(const char *str)
{
	#if CONSOLE_DEV == 1
	serial1Print("log: ");
	serial1Print(str);
	serial1Print("\r\n");
	#elif CONSOLE_DEV == 2
	serial2Print("log: ");
	serial2Print(str);
	serial2Print("\r\n");
	#elif CONSOLE_DEV == 3
	serial3Print("log: ");
	serial3Print(str);
	serial3Print("\r\n");
	#else
	#warning No logging available
	#endif
	sysSleepMs(10);
}

#endif
