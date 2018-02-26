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
//设置帧
#pragma pack(1)		
struct DataLCDTxType1{					//数			//还要注意转义字符的检查 0xA5 0xA6 0x5A 0x5B
    uint8_t	 	Startflag[20];     	 	//20  		 	8个0xA5 +屏地址(2)+源地址(2)+保留字(5)+显示模式(1)+设备类型(1)+协议号(1) 
	uint16_t	DateLengh;				//2字节			从lengh之后到校验位之前
	uint16_t	DateTemp[7];			//				
	uint16_t	DateLengh2;	
	uint8_t		DateTemp2[23];
	uint32_t	DateLengh3;
	uint8_t		*Date;					//				数据域
	uint16_t	Check;			//			 	CRC16	包头数据到数据域
	uint16_t	End;					//1 			5A 
};
#pragma pack()//重置为默认对齐方式

/*
//	封帧	
	0xA5		转义为	0xA6 0x02
	0xA6		转义为	0xA6 0x01
	0x5A		转义为	0x5B 0x02
	0x5B		转义为	0x5B 0x01
	解帧
	同上 反转义
*/	
//包头
#pragma pack(1)	
struct DataPackTxType2{					//数			//还要注意转义字符的检查 0xA5 0xA6 0x5A 0x5B
    uint8_t 	Header[20];	   	 		//2   		 	???LCDAdress SourceAdress Keep DisplayMode
	uint16_t	DateLengh;				//

	
	uint16_t	End;					//1 			5A 
};
#pragma pack()//重置为默认对齐方式


#endif
