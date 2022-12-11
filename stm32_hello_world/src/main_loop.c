/*
main_loop.c

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

#include "version.h"
#include "dev_cfg.h"
#include "utility_functions.h"
#include "serial_dev.h"
#include "sys_dev.h"
#ifdef USB_ENABLE
#include "usbd_cdc_if.h"
#endif
#include "main_loop.h"



//#include "application.h"



#define LOG_PREFIX "ml "
#define LOG_SUFIX "\n"


static int64_t mainTimeInTicks=0;



static int tick_state=0;

static const char *version_string="@(#)" VERSION_STRING;

static int32_t mainSecondsTimer_ms=0;

static int32_t secAndLogSeconds = 0;
static int main_seconds_state = 0;


static void debugLog(const char *str)
{
	char tmp[256];
	int n = 0;
	n += utility_strccpy(tmp+n, LOG_PREFIX, sizeof(tmp)-n);
	n += utility_strccpy(tmp+n, str, sizeof(tmp)-n);
	n += utility_strccpy(tmp+n, LOG_SUFIX, sizeof(tmp)-n);
	SYS_ASSERT(n < sizeof(tmp));
	serialLog(tmp, n);
}

static void debugLog1(const char *str, int i1)
{
	char tmp[256]="";
	int n = 0;
	n += utility_strccpy(tmp+n, str, sizeof(tmp)-n);
	n += utility_strccpy(tmp+n, " ", sizeof(tmp)-n);
	n += utility_itoa(i1, tmp+n, 10);
	debugLog(tmp);
	SYS_ASSERT(n < sizeof(tmp));
}



/**
 * This method shall be called once per tick.
 * TODO Move counting of seconds from main_loop to here.
 * Here we put things that need to be done only once per second.
 * This to spread the CPU load over time. */
void main_seconds_tick()
{
	switch(main_seconds_state)
	{
		case 0:
		{
			const int32_t timerTicks_ms = sysGetSysTimeMs();
			const int32_t time_diff = timerTicks_ms - mainSecondsTimer_ms;
			if (time_diff>=0)
			{
				// One second has passed since last second tick. Increment seconds timer.
				++secAndLogSeconds;
				mainSecondsTimer_ms += 1000;

				SYS_LED_ON();

				// Start a seconds tick sequence.
				// Functions needing seconds tick will get it in the following few calls
				// to even out the CPU load.
				main_seconds_state++;
			}
		}
		break;

		case 1:
		{
			// Put stuff that needs to be called once per second here and in following cases.
			main_seconds_state++;
			break;
		}
		case 2:
		{
			main_seconds_state++;
			break;
		}
		case 3:
		{
			main_seconds_state++;
			break;
		}
		case 4:
		{
			main_seconds_state++;
			break;
		}
		case 5:
		{
			main_seconds_state++;
			break;
		}
		case 6:
		{
			main_seconds_state++;
			break;
		}
		case 7:
		{
			main_seconds_state++;
			break;
		}
		case 8:
		{
			main_seconds_state++;
			break;
		}
		case 9:
		{
			if (secAndLogSeconds % 10 == 0)
			{
				debugLog1("", secAndLogSeconds);
			}
			main_seconds_state++;
			break;
		}
		default:
		{
			sysWdtReset();
			main_seconds_state=0;
			SYS_LED_OFF();
			break;
		}
	}
}


int main_loop(void)
{
	sysInit();

	#if (defined USART1_BAUDRATE)
	if (serial1Init()!=0)
	{
		sysErrorHandler(SYS_USART_ERROR);
	}
	else
	{
		serial1Print("USART1\n");
	}
	#endif

	#if (defined USART2_BAUDRATE)
	if (serial2Init()!=0)
	{
		sysErrorHandler(SYS_USART_ERROR);
	}
	else
	{
		serial2Print("USART2\n");
	}
	#endif

	#if (defined USART3_BAUDRATE)
	if (serial3Init()!=0)
	{
		sysErrorHandler(SYS_USART_ERROR);
	}
	else
	{
		serial3Print("USART3\n");
	}
	#endif

	sysSleepMs(100);
	sysWdtReset();

	// Print some message, so we know the program is running.
	debugLog("");
	debugLog(version_string + 4);
	sysBusyWait(1000000);
	debugLog("build " __DATE__ " " __TIME__);
	#ifdef GIT_VERSION
	debugLog("git " GIT_VERSION);
	#endif


	//sysLedFlashCode(SysMorseL);
	sysSleepMs(100);
	sysWdtReset();

	debugLog("");
	debugLog("hello, world");

	sysSleepMs(100);
	sysWdtReset();

#if (defined __linux__)
	linux_sim_init();
	simulated_init();
#endif




	// Initialize the timer used to know when to do medium tick.
	mainTimeInTicks = sysGetSysTimeMs();
	mainSecondsTimer_ms = mainTimeInTicks + 1000;

	// main loop
	for(;;)
	{
		const int32_t timerTicks_ms = sysGetSysTimeMs();

		// Put stuff that need fast tick (must be called as often as possible) here.


		// In the switch we put things that need to be done medium frequently, this too tries to spread CPU load over time.

		switch(tick_state)
		{
		case 0:
			if (mainTimeInTicks != timerTicks_ms)
			{

				// Timer has been incremented,
				// count ticks and start the tick_state sequence,
				// typically this happens at most once per milli second.
				mainTimeInTicks = timerTicks_ms;
				tick_state++;
			}
			break;
		case 1:
		{
			// Call those that need tick more often than once per second here and in the following cases.
			// This way the CPU load is distributed a bit so not everything happens at same time.
			tick_state++;
			break;
		}
		case 2:
		{
			tick_state++;
			break;
		}
		case 3:
		{
			tick_state++;
			break;
		}
		case 4:
		{
			tick_state++;
			break;
		}
		case 5:
		{
			tick_state++;
			break;
		}
		case 6:
		{
			tick_state++;
			break;
		}
		case 7:
		{
			tick_state++;
			break;
		}
		case 8:
		{
			#if CONSOLE_DEV == 2
			int ch = serial2GetChar();
			switch(ch)
			{
				case -1:
					break;
				case 0x1b: // esc
					debugLog("eternal loop");
					for(;;);
					break;
				default:
					debugLog1("ch",ch);
					break;
			}
			#endif
			tick_state++;
			break;
		}
		case 9:
		{
			// Lastly call the main seconds tick.
			// Those that need a call only once per second will be called from there.
			main_seconds_tick();
			tick_state++;
			break;
		}
		default:
			tick_state=0;
			// Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
			sysSleep();
			break;
		}
	}

	return(0);
} // end main()


