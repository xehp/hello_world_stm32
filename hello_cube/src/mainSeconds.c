/*
mainSeconds.c

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

2016-09-22
Created.
Henrik Bjorkman

*/

#include "sys_time.h"
#include "serialDev.h"
#include "utility.h"
#include "mainSeconds.h"





int32_t secAndLogSeconds = 0;
char superviceState = 0;
int32_t superviceSeconds = 0;
//static int16_t supervice_previous_ac_count=0;



// Log frequency current and voltage at least this often.
#define MIN_LOG_INTERVAL_SEC 60L

// But not more often than this.
#define MINIMUM_TIME_BETWEEN_LOG_S 5



int32_t secAndLogGetSeconds()
{
	return secAndLogSeconds;
}

/**
 * This is expected to be called from main loop once per second
 */
void secAndLogIncSeconds(void)
{
	secAndLogSeconds++;
}




void secAndLogInit(void)
{
	logLine("mainSeconds: secAndLogInit");
}



/** 
 * This method shall be called once per tick.
 * TODO Move counting of seconds from main_loop to here.
 * Here we put things that need to be done only once per second. 
 * This to spread the CPU load over time. */
void secAndLogMediumTick(void)
{
	switch(superviceState)
	{
		case 0:
		if (secAndLogSeconds!=superviceSeconds)
		{
			// Seconds has ticked.

			SYS_LED_ON();

			superviceSeconds=secAndLogSeconds;

			// Start a seconds tick sequence.
			// Functions needing seconds tick will get it in the following few calls
			// to even out the CPU load.
			superviceState++;
		}
		break;

		case 1:
		{
			superviceState++;
			break;
		}
		case 2:
		{
			superviceState++;
			break;
		}
		case 3:
		{
			superviceState++;
			break;
		}
		case 4:
		{
			superviceState++;
			break;
		}
		case 5:
		{
			superviceState++;
			break;
		}
		case 6:
		{
			superviceState++;
			break;
		}
		case 7:
		{
			superviceState++;
			break;
		}
		case 8:
		{
			superviceState++;
			break;
		}
		case 9:
		{
			char tmp[64]="mainSeconds: ";
			utility_lltoa(superviceSeconds, tmp+13, 10/*, sizeof(tmp)-13*/);
			logLine(tmp);
			superviceState++;
			break;
		}
		default:
		{
            sysWdtReset();
			superviceState=0;
			SYS_LED_OFF();
			break;
		}
	}
}
