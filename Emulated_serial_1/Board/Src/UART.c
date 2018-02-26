#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "uart.h"
#include "board.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

///��board.h���г�ʼ���ˣ�����ļ���Ҫ��
/*******************************************************************************
* ������  : UART_GPIO_Configuration
* ����    : UART GPIO��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : USART1 TX:PA9 	RX:PA10
			USART2 TX:PA2 	RX:PA3
			USART3 TX:PB10 	RX:PB11
			USART4 TX:PC10 	RX:PC11
			USART5 TX:PC12 	RX:PD2
*************************************************/
void USART_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* ʹ��APB2ʱ�� */
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB  |
                            RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD  |
                            RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1, ENABLE);

    /* ʹ��APB1ʱ�� */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3  |
                            RCC_APB1Periph_UART4  | RCC_APB1Periph_UART5	, ENABLE);

    /* USART-TX ������� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                          //IO����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO�ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                    //IO�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                             //USART���

    /* USART-Rx �������� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                         //IO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //IO��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                             //USART����IO
}

/************************************************
???? : USART_Configuration
?    ? : ??USART
?    ? : ?
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

    /******************************************************************
     USART?????:  ???     ????   ????  ????
                      115200         8          1      0(NO)
    *******************************************************************/
    USART_InitStructure.USART_BaudRate = 115200;                       //??????
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //????????
    USART_InitStructure.USART_StopBits = USART_StopBits_1;             //???????
    USART_InitStructure.USART_Parity = USART_Parity_No ;               //?????
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//??????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //?????????
    USART_Init(USART1, &USART_InitStructure);                          //???USART1

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                     //??USART1????

    USART_Cmd(USART1, ENABLE);                                         //??USART1
}

/************************************************
???? : USART_Initializes
?    ? : ?????
?    ? : ?
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void USART_Initializes(void)
{
    USART_GPIO_Configuration();
    USART_Configuration();
}

/************************************************
???? : USART1_SendChar
?    ? : ??1??????
?    ? : Data --- ??
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void USART1_SendByte(uint8_t Data)
{
    while((USART1->SR & USART_FLAG_TXE) == RESET);
    USART1->DR = (Data & (uint16_t)0x01FF);
}

/************************************************
???? : USART1_SendNByte
?    ? : ??1??N???
?    ? : pData ----- ???
            Length --- ??
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void USART1_SendNByte(uint8_t *pData, uint16_t Length)
{
    while(Length--)
    {
        USART1_SendByte(*pData);
        pData++;
    }
}

/************************************************
???? : USART1_Printf
?    ? : ??1????
?    ? : string --- ???
? ? ? : ?
?    ? : strongerHuang
*************************************************/
void USART1_Printf(uint8_t *String)
{
    while((*String) != '\0')
    {
        USART1_SendByte(*String);
        String++;
    }
}



/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

