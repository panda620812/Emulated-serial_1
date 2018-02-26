#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "cmsis_os2.h"
#include "board.h"
#include "stm32f10x.h"
#include "ausart.h"

int i ;
float value[4];
uint16_t temp;
float temp1;
float temp2;
float temp3;
float temp4;
float temp5;

void AdcProcess(void);

#define countof(a)   (sizeof(a) / sizeof(*(a)))

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


/*----------------------------------------------------------------------------
 * Application main thread（线程）
 *---------------------------------------------------------------------------*/
 
 __NO_RETURN void app_main (void *argument) //osThreadNew 的主线程，或者直接在main中创建线程
{
//	uint8_t Ledflag = 0;
// 	io_Semaphore = osSemaphoreNew(1, 1, NULL);//semaphore 

	
//	sid_Thread_Semaphore = osSemaphoreNew(2, 2, NULL);

//	if (!sid_Thread_Semaphore) 
//	{
//		; // Semaphore object not created, handle failure
//	}  
	BSP_Init(); 
	VirtualCOM_Config(_9600BuadRate);             //??IO?????????600
	
	while(1)
	{
		{
			thread_gps ()   ; 
			osDelay(1);
		}
	}
}
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *   FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}



