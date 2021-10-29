/*
main.c

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

History

2016-08-15 Created for dielectric test equipment. /Henrik
2021-10-26 Cutting away most code to make a Hello World program. /Henrik
*/

//#include <stdio.h>
//#include <unistd.h>
//#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cfg.h"
#include "sys.h"
#include "version.h"
#include "mainSeconds.h"
#include "serialDev.h"





static int64_t mainTimeInTicks=0;

static int32_t mainSecondsTimer_ms=0;

static int32_t mainLoopCounter=0; // This increments about once per 10 uS

static char tickState=0;






int main(void)
{
	sysInit();

	sysSleepMs(100);

	#if (defined USART1_BAUDRATE)
	if (serialInit(DEV_USART1, USART1_BAUDRATE)!=0)
	{
		sysErrorHandler(SYS_USART1_ERROR);
	}
	else
	{
		serialPrint(DEV_USART1, "USART1\n");
	}
	#endif

	#if (defined USART2_BAUDRATE)
	if (serialInit(DEV_USART2, USART2_BAUDRATE)!=0)
	{
		sysErrorHandler(SYS_USART2_ERROR);
	}
	else
	{
		serialPrint(DEV_USART2, "USART2\n");
	}
	#endif

	#if (defined USART3_BAUDRATE)
	if (serialInit(DEV_USART3, USART3_BAUDRATE)!=0)
	{
		sysErrorHandler(SYS_USART3_ERROR);
	}
	else
	{
		serialPrint(DEV_USART3, "USART3\n");
	}
	#endif

	sysSleepMs(100);



	// Print some message, so we know the program is running.
	logLine("");
	logLine("hello, world");
	logLine("");
	logLine(VERSION_STRING);
	logLine("build " __DATE__ " " __TIME__);
	#ifdef GIT_VERSION
	logLine("git " GIT_VERSION);
	#endif
	#ifdef __arm__
		logLine("arm");
	#endif
	logLine("Copyright 2021 (C) Henrik Bjorkman www.eit.se/hb");
	logLine("GNU GENERAL PUBLIC LICENSE Version 2");

	serialPrint(DEV_USART1, "RCC->CFGR ");
	serialPrint(DEV_USART2, "RCC->CFGR ");
	serialPrintHex16(1, RCC->CFGR);
	serialPrintHex16(2, RCC->CFGR);
	logLine("");



	sys_wdt_reset();

#if (defined __linux__)
	linux_sim_init();
	simulated_init();
#endif



	sysSleepMs(100);

	sys_wdt_reset();



	// Initialize the processes that tick once per second and also does the logging.
	logLine("main: secAndLogInit");
	secAndLogInit();

	sys_wdt_reset();


	logLine("main: Enter main loop");

	// Initialize the timer used to know when to do medium tick.
	mainTimeInTicks=sysGetSysTimeMs();
	mainSecondsTimer_ms=mainTimeInTicks+1000;

	// main loop
	for(;;)
	{

		const int64_t timerTicks_ms=sysGetSysTimeMs();

		// In the switch we put things that need to be done medium frequently, this too tries to spread CPU load over time.

		switch(tickState)
		{
		case 0:
			if (mainTimeInTicks != timerTicks_ms)
			{

				// Timer has been incremented,
				// count ticks and start the tickState sequence,
				// typically this happens once per milli second.
				//mainTimeInTicks++;
				mainTimeInTicks = timerTicks_ms;
				tickState++;
			}
			break;
		case 1:
		{
			// Call those that need tick more often than once per second in the following cases.
			tickState++;
			break;
		}
		case 2:
		{
			const int16_t t = timerTicks_ms-mainSecondsTimer_ms;
			if (t>=0)
			{
				// One second has passed since last second tick. Increment seconds timer.
				secAndLogIncSeconds();
				mainSecondsTimer_ms += 1000;
			}
			tickState++;
			break;
		}
		case 3:
		{
			tickState++;
			break;
		}
		case 4:
		{
			secAndLogMediumTick();
			tickState++;
			break;
		}
		case 5:
		{
			tickState++;
			break;
		}
		case 6:
		{
			tickState++;
			break;
		}
		case 7:
		{
			tickState++;
			break;
		}
		case 8:
		{
			tickState++;
			break;
		}
		case 9:
		{
			tickState++;
			break;
		}
		default:
			// Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
			sysSleep();
			tickState=0;
			break;
		}

		mainLoopCounter++;

		sys_wdt_reset();
	}



	return(0);
} // end main()


