#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "ff_time.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_bkp.h"

s8 DateTimeNow[32];
s8 DateNow[16];
s8 TimeNow[16];
time_t TimeStamp;
time_t kernelTime;
FF_TimeStruct_t   systmtime;

time_t FreeRTOS_time( time_t *pxTime )
{
	time_t uxTime;
	uxTime = kernelTime;
	if( pxTime != NULL )
	{
		*pxTime = uxTime;
	}
	return uxTime;
}



void vApplicationTickHook( void )
{
	static short ticks;
	ticks ++;
	if(ticks > 999)
	{
		ticks = 0;
		kernelTime ++;
		TimeStamp = kernelTime;
	}
}

void vApplicationIdleHook(void)//空闲任务，Freertos要求必须至少有一个任务在运行
{
	static u32 ts_last;
	if(ts_last != TimeStamp)
	{
		ts_last = TimeStamp;
	//	FreeRTOS_localtime_r(&TimeStamp, &systmtime);??
		sprintf((char*)&DateTimeNow[0], "%4d-%02d-%02d %02d:%02d:%02d", 1900+systmtime.tm_year, 1 + systmtime.tm_mon, systmtime.tm_mday, systmtime.tm_hour, systmtime.tm_min, systmtime.tm_sec);
		sprintf((char*)&DateNow[0], "%02d-%02d-%02d", 1900+systmtime.tm_year, 1 + systmtime.tm_mon, systmtime.tm_mday);
		sprintf((char*)&TimeNow[0], "%02d:%02d:%02d", systmtime.tm_hour, systmtime.tm_min, systmtime.tm_sec);
	}
}


