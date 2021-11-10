/*
Barebone blinky on nucleo L432KC / stm32l432kc using SysTick. No RTOS needed.

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

Read more:
https://www.st.com/content/ccc/resource/technical/document/application_note/2e/79/0c/5a/7d/70/4a/c7/DM00049931.pdf/files/DM00049931.pdf/jcr:content/translations/en.DM00049931.pdf


References
[1] DocID027295 Rev 3, https://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/b0/ac/3e/8f/6d/21/47/af/DM00151940/files/DM00151940.pdf/jcr:content/translations/en.DM00151940.pdf
[6] DS11451 Rev 4, https://www.st.com/resource/en/datasheet/stm32l432kb.pdf
[7] RM0038 Rev 16


TODO Rename this file to sysMain or something, since now there are also more generic files like "system_stm32l1xx.c".
*/


#include "sys.h"

// Some macros for backward compatibility with old HAL macros. These can be removed later.
/*
#ifndef TIM_EGR_BG_Msk
#define TIM_EGR_BG_Msk TIM_EGR_BG
#endif
#ifndef RCC_AHBENR_GPIOAEN_Msk
#define RCC_AHBENR_GPIOAEN_Msk RCC_AHBENR_GPIOAEN
#endif
#ifndef RCC_AHBENR_GPIOBEN_Msk
#define RCC_AHBENR_GPIOBEN_Msk RCC_AHBENR_GPIOBEN
#endif
#ifndef RCC_AHBENR_GPIOCEN_Msk
#define RCC_AHBENR_GPIOCEN_Msk RCC_AHBENR_GPIOCEN
#endif
#ifndef RCC_AHBENR_GPIODEN_Msk
#define RCC_AHBENR_GPIODEN_Msk RCC_AHBENR_GPIODEN
#endif
#ifndef RCC_AHBENR_GPIOFEN_Msk
#define RCC_AHBENR_GPIOFEN_Msk RCC_AHBENR_GPIOFEN
#endif
*/

volatile int64_t SysTickCountMs = 0;
volatile int32_t sysDummy = 0;

// Simplest possible SysTick ISR (interrupt service routine).
// TODO This "__attribute__ ((interrupt, used))" what does it do? 
// It seems to work also without it.
// Check that SysTick_Handler exist in the startup_xxx.s file used.
#if 1
void __attribute__ ((interrupt, used)) SysTick_Handler(void)
{
	SysTickCountMs++;
}
#else
void SysTick_Handler(void)
{
	SysTickCountMs++;
}
#endif


// Get the 64 bit timer counter value, be sure to not get a half updated value.
int64_t sysGetSysTimeMs()
{
	int64_t tmp1 = SysTickCountMs;
	int64_t tmp2 = SysTickCountMs;
	while(tmp1 != tmp2)
	{
		tmp1 = SysTickCountMs;
		tmp2 = SysTickCountMs;
	}
	return tmp1;
}

/**
This can be used if a delay is needed before the SysTick is started.
Once sys tick is running use systemSleepMs instead.
Very roughly delay is measured in uS.
 */
void sysBusyWait(uint32_t delay)
{
	while (delay > 0)
	{
		sysDummy++; // Change a volatile, just to be sure the compiler does not optimize this out.
		delay--;
	}
}

// Setup the port/pin that is connected to the LED (Using the Green LED, it works).
// It is assumed that the clock for the port is enabled already.
void sysPinOutInit(GPIO_TypeDef* port, int pin)
{
	// 01: General purpose output mode
	port->MODER &= ~(3U << (pin*2)); // Clear the 2 mode bits for this pin.
	port->MODER |= (1U << (pin*2)); // 1 for "General purpose output mode".

	// 0: Output push-pull (reset state)
	// 0 is reset state -> so nothing needs to be done.
	//port->OTYPER &= ~(0x1U << (pin*1)); // Clear output type, 0 for "Output push-pull".

	// 01: Medium speed
	// 11: High speed
	port->OSPEEDR &= ~(0x3U << (pin*2)); // Clear the 2 mode bits for this pin.
	port->OSPEEDR |= (0x1U << (pin*2)); // 1 for "Medium speed" or 2 for High speed.

	// 00: No pull-up, pull-down
	// 0 is reset state -> so nothing needs to be done.
	//port->PUPDR &= ~(3U << (pin*2)); // Clear the 2 pull up/down bits. 0 for No pull-up, pull-down
}

void sysPinOutSetHigh(GPIO_TypeDef* port, int pin)
{
	port->ODR |= (0x1U << pin);
}

void sysPinOutSetLow(GPIO_TypeDef* port, int pin)
{
	port->ODR &= ~(0x1U << pin);
}


/**
 * Use SysTick as time base source and configure 1ms tick (default clock after Reset is MSI)
 * Configure the SysTick to have interrupt once every 1ms.
 */
static void sysTickInit()
{
	//const uint32_t sysTickPriority = 0xfU; // 0-255, Lower number for sysTickPriority is higher priority.
	//const uint32_t SubPriority = 0;

	/* set reload register */
	SysTick->LOAD  = (uint32_t)(((SysClockFrequencyHz+500)/1000) - 1UL); // May need some calibration.
	SysTick->VAL = 0UL;

	/* Enable SysTick IRQ and SysTick Timer */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
			SysTick_CTRL_TICKINT_Msk   |
			SysTick_CTRL_ENABLE_Msk;

	// Set SysTick priority. Do not know what NVIC_GetPriorityGrouping does. HAL did it that way.
	//uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	//NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, sysTickPriority, SubPriority));
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */

	// It can be good with a tiny delay here for clocks to start up.
	sysBusyWait(10);
}


/* 
Set Interrupt Group Priority
ref [1] does not say anything about SCB->AIRCR.
It must be an ARM/Cortex thing and not an STM32 thing.
http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihehdge.html
 */
static void sysVectorInit()
{
	uint32_t tmp =  SCB->AIRCR;
	const uint32_t VECTKEY = 0x5FAUL; // On writes, write 0x5FA to VECTKEY, otherwise the write is ignored.
	tmp &= ~((uint32_t)((0xFFFFUL << SCB_AIRCR_VECTKEY_Pos) | (7UL << SCB_AIRCR_PRIGROUP_Pos)));
	tmp |= ((uint32_t)VECTKEY << SCB_AIRCR_VECTKEY_Pos) |  (NVIC_PRIORITYGROUP_4 << SCB_AIRCR_PRIGROUP_Pos);
	SCB->AIRCR =  tmp;
}


// Ref [1] chapter 5.3.4 Sleep mode
void sysSleep()
{  
	/* Clear SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

	/* Request Wait For Interrupt */
	__WFI();
}


/**
 * Sleep given amount of milliseconds.
 */
void sysSleepMs(int32_t timeMs)
{
	const int32_t startTimeMs = sysGetSysTimeMs();
	for(;;)
	{
		sysSleep();
		const int32_t currentTimeMs = sysGetSysTimeMs();
		const int32_t timePassedMs = currentTimeMs - startTimeMs;
		if (timePassedMs>=timeMs)
		{
			break;
		}
	}
}

// Find the first bit in error Code that is one.
static int findFirstOne(int errorCode)
{
	for(int i=31; i>=0; i--)
	{
		if ((errorCode >> i) & 1)
		{
			return i;
		}
	}
	return -1;
}

static void flashLed(int base, int len)
{
	#ifdef __arm__
	SYS_LED_ON();
	sysSleepMs(base*len);
	SYS_LED_OFF();
	sysSleepMs(base);
	#else
	if (len == 1)
		printf(".");
	else
		printf("-");
	#endif
}


/**
The error code will blink on the system debug LED.
The error code will be displayed in binary sort of Morse code.
Leading zeroes are not flashed.
For example 8 (binary 1000) will be one one long and 3 short "-...", "daa di di di", morse code for 'B'.
Zero is one short
So positive numbers (>0) all begin with a long (the first bit flashed is a one).
Negative numbers will have a short first to tell that its a negative number.
 -14  .---  J
 -13  .--.  P
 -12  .-.-
 -11  .-..  L
 -10  ..--
  -9  ..-.  F
  -8  ...-  V
  -7  ....  H
  -6  .--   W
  -5  .-.   R
  -4  ..-   U
  -3  ...   S
  -2  .-    A
  -1  ..    I
   0  .     E
   1  -     T
   2  -.    N
   3  --    M
   4  -..   D
   5  -.-   K
   6  --.   G
   7  ---   O
   8  -...  B
   9  -..-  X
  10  -.-.  C
  11  -.--  Y
  12  --..  Z
  13  --.-  Q
  14  ---.
  15  ----
 */
static void flashCode(int code)
{
	const int beep = 200; // length of short beep in ms

	sysSleepMs(beep);

	if (code>0)
	{
		flashLed(beep, 3);
	}
	else
	{
		flashLed(beep, 1);
		if (code<0)
		{
			code=1-code;
		}
	}

	int n = findFirstOne(code);
	for(int i=n-1; i>=0; i--)
	{
		const uint32_t m = 1<<i;
		if ((code & m) == 0)
		{
			flashLed(beep, 1);
		}
		else
		{
			flashLed(beep, 3);
		}
	}
	sysSleepMs(beep);
}

/**
 * Call this if there is an assert error or such. This code depends on system timer to be running.
 * If error happens before that use something else.
 *
 * Stop PWM outputs etc here. That is stop all users activated timers (because they may be driving
 * something and now we need to stop everything).
 */
void sysErrorHandler(int errorCode)
{
	// Do not turn system timer off, systemFlashCode needs it.

	// And/or make sure application calls its own error handler before this is called.
	#if (defined STM32L432xx) || (defined STM32F303x8)
		TIM1->EGR |= TIM_EGR_BG_Msk;
		TIM2->EGR |= TIM_EGR_BG_Msk;
	#elif (defined STM32L151xB)
		TIM2->CR1=0;
        /* TODO Disable all interrupts, except system timer clock */
		//RCC->CIR = 0x00000000;
	#else
	#error Unknown target MCU
	#endif

	SYS_LED_OFF();

	sysSleepMs(1000);

	// Disable external interrupts.
	NVIC->ICER[0]=0xFFFFFFFFUL;
	NVIC->ICER[1]=0xFFFFFFFFUL;
	NVIC->ICER[2]=0xFFFFFFFFUL;

	// Eternal loop (needs system timer to be still running)
	for(;;)
	{
		flashCode(errorCode);
	}
}


// System Clock Config

#if (defined STM32L432xx)


static void sysPeripheralClockStart()
{
	/* Disable all interrupts */
	RCC->CIER = 0x00000000U;

	/*
	AHB2  clock enable register (RCC_AHB2ENR)
	If other ports than GPIOA and GPIOB are to be used those also need
	their clocks to be enabled like this.
	  	  RCC->AHBENR |= RCC_AHBENR_GPIOCEN_Msk;
	  	  RCC->AHBENR |= RCC_AHBENR_GPIODEN_Msk;
	  	  RCC->AHBENR |= RCC_AHBENR_GPIOEEN_Msk;
	 */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN_Msk | RCC_AHB2ENR_GPIOAEN_Msk ;

	// It seems to be good with a tiny delay here and there for clocks etc to start up.
	sysBusyWait(10);
}

#elif (defined STM32F303x8) || (defined STM32L151xB)

static void sysPeripheralClockStart()
{
	/* Disable all interrupts */
	//RCC->CIR = 0x00000000U;


	/* [7] chapter 6.3.8 "AHB peripheral clock enable register (RCC_AHBENR)"
	  AHB peripheral clock enable register (RCC_AHBENR)
	  If for ports to be used the peripheral clock need to be enabled, like this: */
	// TODO To save power comment out those ports that are not used.
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN_Msk;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN_Msk;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN_Msk;
	//RCC->AHBENR |= RCC_AHBENR_GPIOFEN_Msk;


	// If something is not working? Try enable everything, might help or not.
	// Doing this or not should not be needed so comment this out normally.
	// TODO To save power the following shall be commented out when code works.
	//RCC->AHBENR |= 0x31FF0077U;
	//RCC->APB2ENR |= 0x0017F801U;
	//RCC->APB1ENR |= 0x76FEC837U;



	// It seems to be good with a tiny delay here and there for clocks etc to start up.
	sysBusyWait(10);
}

#else
#error not implemented yet
#endif




// To be called from main, to do more setting up
void sysInit(void)
{
	// Do more setting up, stuff not made by SystemInit in "system_stm32l1xx.c" etc.

	sysPeripheralClockStart();

	// Initialize the system debug LED
	sysPinOutInit(SYS_LED_PORT, SYS_LED_PIN);

	sysVectorInit();

	sysTickInit();
}

