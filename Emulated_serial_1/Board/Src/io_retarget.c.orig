#include <stdio.h>
#include "stm32f10x.h"
#include "cmsis_os2.h"
#include "stm32f10x_usart.h"


/*** 重定向标准输入、标准输出、标准错误、tty  ***/

//int stderr_putchar (int ch)
//{
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) osThreadYield();
//	USART_SendData(USART1, (uint8_t) ch);
//	return ch;
//}

//int stdout_putchar (int ch)
//{
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) osThreadYield();
//	USART_SendData(USART1, (uint8_t) ch);
//	return ch;
//}

//int stdin_getchar (void)
//{
//	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) osThreadYield();
//	return USART_ReceiveData(USART1);
//}

//void ttywrch (int ch)
//{
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) osThreadYield();
//	USART_SendData(USART1, (uint8_t) ch);
//}

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数   ，没有使用微库时添加struct _FILE 和_sys_exit              
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f) //重定义到了UART4
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) osThreadYield();
	USART_SendData(USART3, (uint8_t) ch);
	return ch;
}

int fgetc(FILE *f) 
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET) osThreadYield();
	return USART_ReceiveData(USART3);
}

