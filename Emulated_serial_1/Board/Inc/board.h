#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f10x.h"
#include "LED.h"                  // ::Board Support:LED
#include "Buttons.h"              // ::Board Support:Buttons
#include "time.h"
#include "cmsis_os2.h" 
#include "usart.h"

extern s8 DateTimeNow[32];
extern s8 DateNow[16];
extern s8 TimeNow[16];


#define  DebugLog(...)   	osSemaphoreAcquire (io_Semaphore, osWaitForever);\
							printf(__VA_ARGS__);\
                            printf("\r\n");\
							osSemaphoreRelease (io_Semaphore);

#define UART_RX_LEN     256  
extern uint8_t DMA1_CH6_BUF[UART_RX_LEN];
extern osSemaphoreId_t io_Semaphore;


extern void hw_board_init(void);


#endif /* __BOARD_H__ */
