#ifndef	_LCD_H
#define _LCD_H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"				
#include "usart.h"
#include "adc.h"
#include "board.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>
#include "crc8_16.h"
#include "stm32f10x_crc.h"
//����֡
#pragma pack(1)		
struct DataLCDTxType1{					//��			//��Ҫע��ת���ַ��ļ�� 0xA5 0xA6 0x5A 0x5B
    uint8_t	 	Startflag[20];     	 	//20  		 	8��0xA5 +����ַ(2)+Դ��ַ(2)+������(5)+��ʾģʽ(1)+�豸����(1)+Э���(1) 
	uint16_t	DateLengh;				//2�ֽ�			��lengh֮��У��λ֮ǰ
	uint16_t	DateTemp[7];			//				
	uint16_t	DateLengh2;	
	uint8_t		DateTemp2[23];
	uint32_t	DateLengh3;
	uint8_t		*Date;					//				������
	uint16_t	Check;			//			 	CRC16	��ͷ���ݵ�������
	uint16_t	End;					//1 			5A 
};
#pragma pack()//����ΪĬ�϶��뷽ʽ

/*
//	��֡	
	0xA5		ת��Ϊ	0xA6 0x02
	0xA6		ת��Ϊ	0xA6 0x01
	0x5A		ת��Ϊ	0x5B 0x02
	0x5B		ת��Ϊ	0x5B 0x01
	��֡
	ͬ�� ��ת��
*/	
//��ͷ
#pragma pack(1)	
struct DataPackTxType2{					//��			//��Ҫע��ת���ַ��ļ�� 0xA5 0xA6 0x5A 0x5B
    uint8_t 	Header[20];	   	 		//2   		 	???LCDAdress SourceAdress Keep DisplayMode
	uint16_t	DateLengh;				//

	
	uint16_t	End;					//1 			5A 
};
#pragma pack()//����ΪĬ�϶��뷽ʽ


#endif
