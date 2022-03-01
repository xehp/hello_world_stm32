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

References
[1] DocID027295 Rev 3, http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/b0/ac/3e/8f/6d/21/47/af/DM00151940/files/DM00151940.pdf/jcr:content/translations/en.DM00151940.pdf
[2] Device Summary STM32L431xx DS11453 Rev 3 (stm32l431kc.pdf)
[3] Reference Manual RM0394 Rev 4 (en.DM00151940.pdf)
[4] RM0038 Rev 16
[5] DocID024330 Rev 5

Ref [4] found at:
https://www.st.com/en/microcontrollers-microprocessors/stm32l151-152.html#documentation
*/

//#include "stm32l1xx_hal_def.h"
/*#if (defined STM32L432xx)
#include "stm32l4xx_hal_gpio_ex.h"
#elif (defined STM32L151xB)
#include "stm32l1xx_hal_gpio_ex.h"
#elif (defined STM32F303x8)
#include "stm32f3xx_hal_gpio_ex.h"
#else
#error Unknown device
#endif*/


#include "sys_time.h"
#ifdef USB_ENABLE
#include "usbd_cdc_if.h"
#endif
#include "utility.h"
#include "fifo.h"
#include "serialDev.h"




#ifdef LPUART1_BAUDRATE
	// LPUART1 is not available in an STM32L1 device but is available in STM32L4.

	// NOTE Two uarts, usarts etc shall not use same pins.
	// For example USART2 and LPUART1 can not both use PA2.
    // NOTE If PA2 and PA3 are used for LPUART1 then USART2 can not also use these pins.

    // LPUART1_TX_PIN can be set to 2 or 11 here for PA2 or PB11 respectively.
	#define LPUART1_TX_PIN 2

    // LPUART1_TX_PIN can be set to 3 or 10 here for PA3 or PB10 respectively.
	#define LPUART1_RX_PIN 3

	volatile struct Fifo lpuart1In = {0,0,{0}};
	#ifdef LPUART1_TX_PIN
	volatile struct Fifo lpuart1Out = {0,0,{0}};
	#endif
#endif

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
	  volatile uint32_t tmp = USART1->ISR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_ISR_RXNE_Msk)
	  {
		// Simple receive, this never happens :(
		//mainDummy = USART1->RDR;
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
	  volatile uint32_t tmp = USART2->ISR;

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

		  // This was just for testing, remove later...
		  //USART2->CR1 |= (USART_CR1_RXNEIE_Msk);
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
		// Simple receive, this never happens :(
		fifoPut(&usart3In, (int)(USART3->RDR & 0xFF));

		// For debugging count the RXNE interrupts.
		//mainCh++; // Remove this when things work.
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

	void __attribute__ ((interrupt, used)) USART1_IRQHandler(void)
	{
	  volatile uint32_t tmp = USART1->SR;

	  // RXNE (Receive not empty)
	  if (tmp & USART_SR_RXNE_Msk)
	  {
		// Simple receive, this never happens :(
		//mainDummy = USART1->RDR;
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

	#ifdef USART2_BAUDRATE
	void __attribute__ ((interrupt, used)) USART2_IRQHandler(void)
	{
	  volatile uint32_t tmp = USART2->SR;

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

		  // This was just for testing, remove later...
		  //USART2->CR1 |= (USART_CR1_RXNEIE_Msk);
		}
	  }
	  #endif
	}
	#endif

	#ifdef USART3_BAUDRATE
	void __attribute__ ((interrupt, used)) USART3_IRQHandler(void)
	{
	  volatile uint32_t tmp = USART3->SR;

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
	  volatile uint32_t tmp = USART1->ISR;

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

		volatile uint32_t tmp = USART2->ISR;

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

				// This was just for testing, remove later...
				//USART2->CR1 |= (USART_CR1_RXNEIE_Msk);
			}
		}
		#endif

		//SYS_LED_OFF();

		// This line is probably not needed.
		USART2->ICR = 0x2A5F;

	}
	#endif

#endif

/* This seems to work fine, we do get what the Nucleo sends */
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
    const uint32_t mode = 2U;
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
    const uint32_t speed = 2U;
    tmp = base->OSPEEDR;
    tmp &= ~(3U << (pin * 2));
    tmp |= (speed << (pin * 2));
    base->OSPEEDR = tmp;

    /* [1]
    Configure the IO Output Type
    0: Output push-pull (reset state)
    1: Output open-drain
    */
    //const uint32_t ot = 2U;
    //tmp = GPIOA->OTYPER;
    //tmp &= ~(1U << (pin * 1));
    //tmp |= (ot << (pin * 1));
    //GPIOA->OTYPER = tmp;

    /* [1]
    Activate the Pull-up or Pull down resistor for the current IO 
    00: No pull-up, pull-down (reset state for most IO pins)
    01: Pull-up
    10: Pull-down
    11: Reserved
    */
    //tmp = GPIOA->PUPDR;
    //tmp &= ~(3U << (pin * 2));
    //tmp |= (GPIO_NOPULL << (pin * 2));
    //GPIOA->PUPDR = tmp;
}

/**
 * Perhaps both TX and RX pins can be configured the same way.
 * If so only one method setupIoPinRxTx should be needed.
 * */
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
    Configure the IO Output Type
    0: Output push-pull (reset state)
    1: Output open-drain
    Some examples use push-pull others open-drain, did not notice any difference.
    This pin is for input anyway so this should not matter.
    */
    //tmp = GPIOA->OTYPER;
    //tmp &= ~(1U << (pin * 1));
    //tmp |= (1U << (pin * 1)); 
    //GPIOA->OTYPER = tmp;

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
    GPIOA->PUPDR = tmp;
}



USART_TypeDef *usartGetPtr(int usartNr)
{
    switch(usartNr)
    {
        case DEV_USART1: return USART1;
        case DEV_USART2: return USART2;
        #ifdef USART3_BAUDRATE
        case DEV_USART3: return USART3;
        #endif
        default :
            sysErrorHandler(SYS_UNKNOWN_UART_ERROR);
            break;
    }
    return NULL;
}

int usartGetIrqN(int usartNr)
{
	switch(usartNr)
	{
		#ifdef LPUART1_BAUDRATE
		case DEV_LPUART1: return LPUART1_IRQn;
		#endif
		case DEV_USART1: return USART1_IRQn;
		case DEV_USART2: return USART2_IRQn;
		#ifdef USART3_BAUDRATE
		case DEV_USART3: return USART3_IRQn;
		#endif
		default :
			sysErrorHandler(SYS_UNKNOWN_UART_ERROR);
			break;
	}
	return 0;
}


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


	// Configure IO pins, to find pins and alternate function codes for STM32L432xx see table 17 in ref [2].
	// On Nucleo L432KC we use Serial2 on PA2 & PA15
	// So PA3 is free to use for LPUART1 but we have no transmit pin available.
	// TODO Use LPUART1 for receiving and SoftUart1 for transmitting.

	// Use PA2 for TX & PA3 for RX
	#if LPUART1_TX_PIN==2
	setupIoPinTx(GPIOA, 2, GPIO_AF8_LPUART1);
	#elif LPUART1_TX_PIN==11
	setupIoPinTx(GPIOB, 11, GPIO_AF8_LPUART1);
	#else
	#error
	#endif

	#if LPUART1_RX_PIN==3
	setupIoPinRx(GPIOA, 3, GPIO_AF8_LPUART1);
	#elif LPUART1_RX_PIN==10
	setupIoPinRx(GPIOB, 10, GPIO_AF8_LPUART1);
	#else
	#error
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


#ifdef USART1_BAUDRATE
static void setup_fifo_and_io_pins_usart1()
{
	fifoInit(&usart1In);
	fifoInit(&usart1Out);

	// If higher baud rate is needed change clock source.
	// Select clock LPUART1SEL
	/*{
	 uint32_t tmp = RCC->CCIPR;
	 tmp &= ~(3 << 0);
	 tmp |= ~(1 << 0);
	 RCC->CCIPR = tmp;
	 }*/
	//RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST_Msk; // [RM0434] Bit 14 USART1RST: USART1 reset


	// Enable Usart1 device
	#if (defined STM32L432xx)
	RCC->APB2ENR1 |= RCC_APB2ENR1_USART1EN_Msk;
	#elif (defined STM32L151xB) || (defined STM32F303x8) || (defined STM32F030x8)
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN_Msk;
	#else
	#error Unknown device
	#endif


	// Configure IO pins.
	#if (defined STM32F030x8)
	setupIoPinTx(GPIOA, 9, GPIO_AF1_USART1);
	setupIoPinRx(GPIOA, 10, GPIO_AF1_USART1);
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

}
#endif

#ifdef USART2_BAUDRATE
static void setup_fifo_and_io_pins_usart2()
{
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

	// Enable Usart2 device
	#if (defined STM32L432xx)
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN_Msk;  // bit 17
	#elif (defined STM32L151xB) || (defined STM32F303x8) || (defined STM32F030x8)
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk;
	#else
	#error Unknown device
	#endif

	// Setup transmit pin
	#ifdef USART2_TX_PIN
		#if USART2_TX_PIN == 2
			#if (defined STM32F030x8)
			setupIoPinTx(GPIOA, 2, GPIO_AF1_USART2);
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

	// Setup receive pin
	#ifdef USART2_RX_PIN
		#if USART2_RX_PIN == 3
			#if (defined STM32F0)
			// Want Usart2 RX on PA3 so its AF1
			setupIoPinRx(GPIOA, 3, GPIO_AF1_USART2);
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
}
#endif

#ifdef USART3_BAUDRATE
static void setup_fifo_and_io_pins_usart3()
{
	fifoInit(&usart3In);
	fifoInit(&usart3Out);

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk;

	// For STM32L151xB see table 10 in ref [5].
	// Want Usart3 on TX=PB10 RX=PB11, so its AF7 for both.
	// It is also possible to use PC4, PC5, PC10, PC11 (all AF7) on some devices.
	// See table 16 in ref [2].

	// Transmit pin PB10/AF7
	setupIoPinTx(GPIOB, 10, GPIO_AF7_USART2);

	// Receive pin PB11/AF7
	setupIoPinRx(GPIOB, 11, GPIO_AF7_USART2);
}
#endif

#if (defined USART1_BAUDRATE) || (defined USART2_BAUDRATE) || (defined USART3_BAUDRATE)
/**
This sets up the Usart 1 or 2. 

USART1_TX on PA9, USART1_RX on PA10. 
Usart2 sends on USB (perhaps also on PA2), it receives on PA3.

Returns 0 if OK.
*/
static int initUsart123(int usartNr, uint32_t baud)
{
	USART_TypeDef *usartPtr = usartGetPtr(usartNr);
	if (usartPtr == NULL) 
	{
		return -1;
	}

	/* Disable the Peripheral, should be the reset state but just in case. */
	usartPtr->CR1 &= ~USART_CR1_UE_Msk;

	// Clear the in and out FIFOs
	// Enable Usart clock
	switch (usartNr)
	{
		#if (defined USART1_BAUDRATE)
		case DEV_USART1:
			setup_fifo_and_io_pins_usart1();
			break;
		#endif
		#ifdef USART2_BAUDRATE
		case DEV_USART2:
			setup_fifo_and_io_pins_usart2();
			break;
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3:
			setup_fifo_and_io_pins_usart3();
			break;
		#endif
		default:
			return -1;
	}


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
	uint32_t divider = (SysClockFrequencyHz + (baud/2)) / baud;
	/*if (divider<0x300)
	 {
	 return -1;
	 }*/
	usartPtr->BRR = divider;

	/* Enable uart transmitter and receiver */
	usartPtr->CR1 |= USART_CR1_TE_Msk | USART_CR1_RE_Msk;

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
	usartPtr->CR2 &= ~(USART_CR2_CLKEN_Msk);

	sysBusyWait(2);

	//Set Usart1 interrupt priority. Lower number is higher priority.
	//uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	//NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(prioritygroup, 45, 0));
	const int irqN = usartGetIrqN(usartNr);
	NVIC_SetPriority(irqN, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// This one helped.
	// https://community.st.com/thread/19735
	// I was missing to do NVIC_EnableIRQ.
	NVIC_EnableIRQ(irqN);

	/* Enable the Peripheral */
	usartPtr->CR1 |= USART_CR1_UE_Msk;

	/*
	 Shall interrupt be enabled before or after usart is enabled?
	 Did not notice any difference doing this before or after.
	 RXNEIE
	 0: Interrupt is inhibited
	 1: A USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_ISR
	 */
	usartPtr->CR1 |= USART_CR1_RXNEIE_Msk;

	return 0;
}
#endif

// Returns zero if OK.
int serialInit(int usartNr, uint32_t baud)
{
	switch(usartNr)
	{
		#ifdef LPUART1_BAUDRATE
		case DEV_LPUART1: return lpuartInit(baud);
		#endif
		#ifdef USART1_BAUDRATE
		case DEV_USART1: return initUsart123(usartNr, baud);
		#endif
		#ifdef USART2_BAUDRATE
		case DEV_USART2: return initUsart123(usartNr, baud);
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3: return initUsart123(usartNr, baud);
		#endif
		#ifdef USB_ENABLE
		case DEV_USB:
			// TODO USB is initialized from main.
			break;
		#endif
		default: break;
	}
	return -1;
}


#ifdef LPUART1_TX_PIN
static inline void lpuart1Put(int ch)
{
	fifoPut(&lpuart1Out, ch);
	LPUART1->CR1 |= USART_CR1_TXEIE_Msk;
}
#endif

#if (defined USART1_BAUDRATE)
static inline void usart1Put(int ch)
{
	fifoPut(&usart1Out, ch);
	/*
	Now we need to trigger the ISR. Its done by enabling transmitter empty interrupt (it is empty so).
	TXEIE
	0: Interrupt is inhibited
	1: A USART interrupt is generated whenever TXE=1 in the USART_ISR register
	*/
	USART1->CR1 |= USART_CR1_TXEIE_Msk;
}
#endif

#if (defined USART2_BAUDRATE) && (defined USART2_TX_PIN)
static inline void usart2Put(int ch)
{
	// If FIFO is full shall we wait or not?
	// Comment out lines below if not.
	//while (fifoIsFull(usart2Out))
	//{
	//	systemSleep();
	//}

	fifoPut(&usart2Out, ch);
	// Now we need to trigger the ISR. Its done by enabling transmitter empty interrupt (it is empty so).
	USART2->CR1 |= USART_CR1_TXEIE_Msk;
}
#endif

#ifdef USART3_BAUDRATE
static inline void usart3Put(int ch)
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
#endif


// Ref [1] chapter 36.5.2 USART transmitter
void serialPutChar(int usartNr, int ch)
{
	switch(usartNr)
	{
		#ifdef LPUART1_TX_PIN
		case DEV_LPUART1:
			lpuart1Put(ch);
			break;
		#endif
		#if (defined USART1_BAUDRATE)
		case DEV_USART1:
			usart1Put(ch);
			break;
		#endif
		#ifdef USART2_BAUDRATE
		case DEV_USART2:
			usart2Put(ch);
			break;
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3:
			usart3Put(ch);
			break;
		#endif
		#ifdef USB_ENABLE
		case DEV_USB:
			cdcPutChar(ch);
			break;
		#endif
		default:
			// Ignore this.
		break;
	}
}

int serialGetChar(int usartNr)
{
	switch(usartNr)
	{
		#ifdef LPUART1_BAUDRATE
		case DEV_LPUART1:
			if (!fifoIsEmpty(&lpuart1In))
			{
				return fifoTake(&lpuart1In);
			}
			return -1;
		#endif
		#if (defined USART1_BAUDRATE)
		case DEV_USART1:
			if (!fifoIsEmpty(&usart1In))
			{
				return fifoTake(&usart1In);
			}
			return -1;
		#endif
		#if (defined USART2_BAUDRATE) && (defined USART2_RX_PIN)
		case DEV_USART2:
			if (!fifoIsEmpty(&usart2In))
			{
				return fifoTake(&usart2In);
			}
			return -1;
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3:
			if (!fifoIsEmpty(&usart3In))
			{
				return fifoTake(&usart3In);
			}
			return -1;
		#endif
		#ifdef USB_ENABLE
		case DEV_USB:
			return cdcTakeChar();
		#endif
		default:
			return -1;
		break;
	}
}

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
static inline void usartWriteUart1(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		usart1Put(*str++);
	    msgLen--;
	}
}
#endif

#ifdef USART2_BAUDRATE
static inline void usartWriteUart2(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		usart2Put(*str++);
	    msgLen--;
	}
}
#endif

#ifdef USART3_BAUDRATE
static inline void usartWriteUart3(const char *str, int msgLen)
{
	while (msgLen>0)
	{
		usart3Put(*str++);
	    msgLen--;
	}
}
#endif


void serialWrite(int usartNr, const char *str, int msgLen)
{
	switch(usartNr)
	{
		#ifdef LPUART1_TX_PIN
		case DEV_LPUART1:
			usartWriteLpuart1(str, msgLen);
			break;
		#endif
		#if (defined USART1_BAUDRATE)
		case DEV_USART1:
			usartWriteUart1(str, msgLen);
			break;
		#endif
		#ifdef USART2_BAUDRATE
		case DEV_USART2:
			usartWriteUart2(str, msgLen);
			break;
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3:
			usartWriteUart3(str, msgLen);
			break;
		#endif
		#ifdef USB_ENABLE
		case DEV_USB:
		{
			while (msgLen>0)
			{
				cdcPutChar(*str++);
			    msgLen--;
			}
			break;
		}
		#endif
		default:
			/*
			while (msgLen>0)
			{
			    serialPutChar(usartNr, *str++);
			    msgLen--;
			}
			*/
			// ignore this
			break;
	}
}

void serialPrint(int usartNr, const char *str)
{
	switch(usartNr)
	{
		#ifdef USB_ENABLE
		case DEV_USB:
		{
			const int msgLen = utility_strlen(str);
			serialWrite(usartNr, str, msgLen);
			break;
		}
		#endif
		default:
		while(*str)
		{
			serialPutChar(usartNr, *str++);
		}
		break;
	}
}

void serialPrintInt64(int usartNr, int64_t num)
{
	char str[64];
	utility_lltoa(num, str, 10);
	serialPrint(usartNr, str);
}

int serialGetFreeSpaceWriteBuffer(int usartNr)
{
	switch(usartNr)
	{
		#if (defined LPUART1_BAUDRATE) && (defined LPUART1_TX_PIN)
		case DEV_LPUART1:
			fifoPut(&lpuart1Out, ch);
			return fifo_free_space(&usart1Out);
		#endif
		#if (defined USART1_BAUDRATE)
		case DEV_USART1:
			return fifo_free_space(&usart1Out);
		#endif
		#if (defined USART2_BAUDRATE) && (defined USART2_TX_PIN)
		case DEV_USART2:
			return fifo_free_space(&usart2Out);
		#endif
		#ifdef USART3_BAUDRATE
		case DEV_USART3:
			return fifo_free_space(&usart3Out);
		#endif
		default:
			// Ignore this.
		break;
	}
	return 0;
}


static const int hex[]={'0','1','2','3','4','5','6','7', '8','9','A','B','C','D','E','F'};

void serialPrintHex4(int usartNr, uint8_t i)
{
	serialPutChar(usartNr, hex[i&0xf]);
}

void serialPrintHex8(int usartNr, uint8_t i)
{
	serialPrintHex4(usartNr, i>>4);
	serialPrintHex4(usartNr, i&0xf);
}

void serialPrintHex16(int usartNr, uint16_t i)
{
	serialPrintHex8(usartNr, i>>8);
	serialPrintHex8(usartNr, i&0xff);
}

void serialPrintHex32(int usartNr, uint32_t i)
{
	serialPrintHex16(usartNr, i>>16);
	serialPrintHex16(usartNr, i&0xffff);
}

void serialPrintHex64(int usartNr, uint64_t i)
{
	serialPrintHex32(usartNr, i>>32);
	serialPrintHex32(usartNr, i&0xffffffff);
}


void logLine(const char *str)
{
	#ifdef USART1_BAUDRATE
	serialPrint(DEV_USART1, str);
	serialPrint(DEV_USART1, "\r\n");
	#endif
	#ifdef USART2_BAUDRATE
	serialPrint(DEV_USART2, str);
	serialPrint(DEV_USART2, "\r\n");
	#endif
	#ifdef USB_ENABLE
	serialPrint(DEV_USB, str);
	serialPrint(DEV_USB, "\r\n");
	#endif
	sysSleepMs(50);
}

