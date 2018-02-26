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
/*			USART1 TX:PA9 	RX:PA10  1	//232 
			USART2 TX:PA2 	RX:PA3   1	//485	PA1_RS485_CTR1
			USART3 TX:PB10 	RX:PB11 	//232
			UART4 TX:PC10 	RX:PC11 	//TTL//¿ÉÊÇÊÇ				9600 	PM2.5 PM10
			UART5 TX:PC12 	RX:PD2   1	//485	PB3_RS485_CTR2  	9600 	RFID
			ÆäÖĞ¶ÔÖ÷°åÁ¬½ÓµÄ»¹²»Çå³ş£¬¿ÉÄÜÊÇ
*/
#define   count1 10
extern float temp1;
extern float temp2;
extern float temp3;
extern float temp4;
extern float temp5;


struct DataTxType 			DataTx;				//Êı¾İ·¢ËÍ£¨¶à×´Ì¬1£©
struct DataPMType			DataPm;
struct DataPM25PM10RxType	DataRxPm;			//PM25PM10½ÓÊÕ´æ·Å
struct DataXhpRxType		DataXhpRx;		//ĞÂ»İÆÕ 
struct DataTempPaRxType		DataTempPaRx;	//¦ÂÉäÏß·¨½ÓÊÕÎÂ¶ÈÑ¹Á¦
struct DataRFIDRxType		DataRFIDRx;
struct DataRFIDTxType		DataRFIDTx;
struct DataAWA5636RxType	AWA5636Rx;

//struct PNetTopologyRecord pNet;

//uint8_t uinttemp1 = (uint8_t)temp1;
uint8_t temp_u5 = 0;
//½ÓÊÕÇø»º´æ
uint8_t RxBuf1[100] = {0x00};
uint8_t RxBuf2[100] = {0x00};
uint8_t RxBuf3[100] = {0x00};
uint8_t RxBuf4[100] = {0x00};
uint8_t RxBuf5[100] = {0x00};
//½ÓÊÕÖ¡ÌáÈ¡´æ´¢Êı×é
uint8_t RxBuf1temp[24] = {0x00};
uint8_t RxBuf2temp[24] = {0x00};
uint8_t RxBuf3temp[24] = {0x00};
uint8_t RxBuf4temp[24] = {0x00};
uint8_t RxBuf5temp[24] = {0x00};
//·¢ËÍÇø»º´æ
uint8_t TxBuf1[];
uint8_t TxBuf2[10];
uint8_t TxBuf3[];
uint8_t TxBuf4[];
uint8_t TxBuf5[10];
//½ÓÊÕÖ¡±êÖ¾Î»
uint8_t Rx1Flag = 0;
uint8_t Rx2Flag = 0;
uint8_t Rx3Flag = 0;
uint8_t Rx4Flag = 0;
uint8_t Rx5Flag = 0;
//æ¥æ”¶è®¡æ•°
uint8_t Rx1Count = 0;
uint8_t Rx2Count = 0;
uint8_t Rx3Count = 0;
uint8_t Rx4Count = 0;
uint8_t Rx5Count = 0;

//Baud
uint32_t Baud  = 115200;
uint32_t Baud1 = 115200;
uint32_t Baud2 = 115200;//9600 ÎÂÊª¶È·çÏò  + ÔëÉù
uint32_t Baud3 = 115200;
uint32_t Baud4 = 115200;//9600 PM2.5 PM10	
uint32_t Baud5 = 115200;//9600 RFID

uint32_t Baud1temp1;
uint32_t Baud1temp2;
uint8_t  Baud1temp[3] = {0};

//ĞÂ»İÆÕ
//¿ØÖÆÂë ÓĞÒ»¸öÒ»Ìå»¯·çËÙ·çÏò£¬Êª¶ÈÓĞ¸öÍÁÈÀÊª¶È,ÔÚ¿ª»úÊ±·¢ËÍÒ»¸öÇëÇó(Î´Ìí¼Ó)£¬Ö®ºó½øÈëÑ­»·Ask£¬µ«ÊÇÊ§È¥Ó¦´ğÔõÃ´½â¾ö£¿
uint8_t WindSpeedAsk[4] 	= {0x01,0x80,0x01,0x80};
uint8_t WindDirectAsk[4] 	= {0x01,0x81,0xC0,0x40};	
uint8_t HumidityAsk[4] 		= {0x01,0x82,0x80,0x41};	
uint8_t TempAsk[4] 				= {0x01,0x83,0x41,0x81};	
uint8_t kPaAsk[4] 				= {0x01,0x84,0x00,0x43};	
uint8_t Noise1Ask[4] 			= {0x01,0x99,0xC0,0x4A};
uint8_t Asktemp[4] 				= {0x10,0x80,0x40,00};
uint8_t Judge;				//Ğ£ÑéÅĞ¶Ï
uint8_t	StateFlag = 1;
//AWA5636	ÔëÉù
uint8_t AWAAsk[4] =	{'A','W','A','A'};


uint16_t index1=0,index2=0,index3=0,flag1=0,flag2=0,flag3=0;
uint16_t temp110;
uint16_t temp22 = 0;
uint16_t temp10;
uint16_t temp9;

//PM2.5 PM10 Öµ»ñÈ¡
uint16_t PM2_5;
uint32_t PM10;


void delay_ms(u16 time) ;
uint8_t Baudchange(USART_InitTypeDef USART_InitStructurex,uint8_t baudtempget,USART_TypeDef* USARTx);//BaudĞŞ¸Ä
void DataXhpProcess(void);//ĞÂ»İÆÕÊı¾İ´¦Àí
void TempPaProcess(void);//¦ÂÉäÏß·¨½ÓÊÕÎÂ¶ÈÑ¹Á¦Á÷Á¿²¿·Ö


//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */

	USART_InitTypeDef USART_InitStructure;
	USART_InitTypeDef USART_InitStructure1;
	USART_InitTypeDef USART_InitStructure2;
	USART_InitTypeDef USART_InitStructure3;
	USART_InitTypeDef USART_InitStructure4;	
	USART_InitTypeDef USART_InitStructure5;
/* ÔÚio_retargetÀïÃæÓĞ
#if 1
#pragma import(__use_no_semihosting)             
//±ê×¼¿âĞèÒªµÄÖ§³Öº¯Êı                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êı 
 int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    UART4->DR = (uint8_t) ch;      
	return ch;
}
#endif 
*/
//__NO_RETURN void USART_5 (void *argument)
//{
//	osDelay (1000);
//	
//	USART_Configuration();
//}

/************************************************
º¯ÊıÃû³Æ £º void USART_Configuration()
¹¦    ÄÜ £º ´®¿Ú³õÊ¼»¯
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º ÎŞ
Ëµ	  Ã÷ :	usart3 Óë jtag Òı½Å¸´ÓÃÁË£¬ËùÒÔdisable ,usart2 DMA
*************************************************/
void USART_Configuration()//ÓëÖ÷°åÁ¬½ÓµÄÊÇÒ»¸ö232½Ó¿Ú
{
	//½á¹¹Ìå³õÊ¼»¯
	DataTx.Startflag		=	0xAA;
	DataTx.SendChannel 	=	0x03;//ĞèĞŞ¸Ä
	DataTx.FunctionCode 	= 	0x00;//
	DataTx.DataLengh		=	0x12;//Òò¶ÔÆë·½Ê½ĞèÈ·ÈÏ
	
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	//GPIO¶Ë¿ÚÉèÖÃ
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO 	| 
							RCC_APB2Periph_USART1	|
							RCC_APB2Periph_GPIOA	|
							RCC_APB2Periph_GPIOB 	| 
							RCC_APB2Periph_GPIOC	| 
							RCC_APB2Periph_GPIOD, ENABLE);	//Ê¹ÄÜGPIOÊ±ÖÓ
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART2	|
							RCC_APB1Periph_USART3	|
							RCC_APB1Periph_UART4	|	
							RCC_APB1Periph_UART5,ENABLE);	//Ê¹ÄÜUSART
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//CRCÊ±ÖÓÊ¹ÄÜ
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*Ê¹ÓÃSW½ûÓÃJTAG*/
	
	
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
	GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.9
	//USART1_RX	  GPIOA.10³õÊ¼»¯
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
	GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.10  
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART2_RX	  GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART3_RX	  GPIOB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);/**/
	//UART4_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//UART4_RX	  GPIOC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//UART5_TX   GPIOC.12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//UART5_RX	  GPIOD.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//RS485_CTR1 //PA1//UART2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//RS485_CTR2 //PB3	//UART5	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	// ´®¿Ú3ÖØÓ³Éä
	//GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);//
  /******************************************************************
   USART²ÎÊı³õÊ¼»¯:  ²¨ÌØÂÊ     ´«ÊäÎ»Êı   Í£Ö¹Î»Êı  Ğ£ÑéÎ»Êı
                    115200         8          1      0(NO)
  *******************************************************************/
	USART_InitStructure1.USART_BaudRate = Baud1;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructure1.USART_WordLength = USART_WordLength_8b;					//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure1.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure1.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//ÊÕ·¢Ä£Ê½
	//USART_BRR
	USART_Init(USART1, &USART_InitStructure1); 			//³õÊ¼»¯´®¿Ú1
	USART_Cmd(USART1, ENABLE);                    		//Ê¹ÄÜ´®¿Ú1 
	
	USART_InitStructure2.USART_BaudRate = Baud2;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructure2.USART_WordLength = USART_WordLength_8b;						//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure2.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure2.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//ÊÕ·¢Ä£Ê½	
	
	USART_Init(USART2, &USART_InitStructure2);			//³õÊ¼»¯´®¿Ú2
	USART_Cmd(USART2, ENABLE);							//Ê¹ÄÜ´®¿Ú2 
	
	USART_InitStructure3.USART_BaudRate = Baud3;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructure3.USART_WordLength = USART_WordLength_8b;						//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure3.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure3.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
//  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);		//²ÉÓÃDMA·½Ê½½ÓÊÕ 
//	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	USART_Init(USART3, &USART_InitStructure3); 			//³õÊ¼»¯´®¿Ú3
	USART_Cmd(USART3, ENABLE);                    		//Ê¹ÄÜ´®¿Ú3
	//USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);		//²ÉÓÃDMA·½Ê½½ÓÊÕ 
	
	USART_InitStructure4.USART_BaudRate = Baud4;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructure4.USART_WordLength = USART_WordLength_8b;						//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure4.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure4.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure4.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(UART4, &USART_InitStructure4); 			//³õÊ¼»¯´®¿Ú4
	USART_Cmd(UART4, ENABLE);                    		//Ê¹ÄÜ´®¿Ú4
	
	USART_InitStructure5.USART_BaudRate = Baud5;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructure5.USART_WordLength = USART_WordLength_8b;						//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure5.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure5.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure5.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure5.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(UART5, &USART_InitStructure5); 			//³õÊ¼»¯´®¿Ú5
	USART_Cmd(UART5, ENABLE);                    		//Ê¹ÄÜ´®¿Ú5

	
//	TX_MODE_USART2;
//	TX_MODE_UART5;
	//½ÓÊÕÖĞ¶ÏÊ¹ÄÜ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	  
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	/* ´ò¿ª¿ÕÏĞÖĞ¶Ï */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	/* Ê¹ÄÜ´®¿Ú1 */
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);	
	USART_Cmd(UART4, ENABLE);	
	USART_Cmd(UART5, ENABLE);	
	
	USART_ClearFlag(USART1, USART_FLAG_TC); //·¢ËÍ±êÖ¾Î»ÇåÁã
	USART_ClearFlag(USART2, USART_FLAG_TC); 
	USART_ClearFlag(USART3, USART_FLAG_TC); 
	USART_ClearFlag(UART4,	USART_FLAG_TC);
	USART_ClearFlag(UART5,	USART_FLAG_TC);
}

void USART_PRINT(USART_TypeDef* USARTx, char *Data)
{ 
	while(*Data)
	{
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) osThreadYield();
		USART_SendData(USARTx, (uint8_t) *Data);
		Data++;
	}
}
//TXE=·¢ËÍÊı¾İ¼Ä´æÆ÷¿Õ,TC=·¢ËÍ½áÊø
void USART_SEND(USART_TypeDef* USARTx, s8 *Data, s32 len)
{ 
	while(len)
	{
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) osThreadYield();
		USART_SendData(USARTx, (uint8_t) *Data);
		Data++;
		len--;
	}
}
/************************************************
º¯ÊıÃû³Æ £º USARTx_SendChar
¹¦    ÄÜ £º ´®¿Ú x ·¢ËÍÒ»¸ö×Ö·û
²Î    Êı £º Data --- Êı¾İ
·µ »Ø Öµ £º ÎŞ
×÷    Õß £º strongerHuang
*************************************************/
void USARTx_SendByte(USART_TypeDef* USARTx	,	uint8_t Data)
{
  while((USARTx->SR & USART_FLAG_TXE) == RESET);
  USARTx->DR = (Data & (uint16_t)0x01FF);//Ö»ÓĞµÍ9Î»ÓĞĞ§£¬Ö÷Òª·ÀÖ¹¸ÉÈÅ
}

/************************************************
º¯ÊıÃû³Æ £º USARTx_SendNByte
¹¦    ÄÜ £º ´®¿Úx·¢ËÍN¸ö×Ö·û
²Î    Êı £º pData ----- ×Ö·û´®
            Length --- ³¤¶È
·µ »Ø Öµ £º ÎŞ
×÷    Õß £º strongerHuang
*************************************************/
void USARTx_SendNByte(USART_TypeDef* USARTx	,uint8_t *pData, uint16_t Length)
{
  while(Length--)
  {
    USARTx_SendByte(USARTx,*pData);
    pData++;
  }
}
/************************************************
º¯ÊıÃû³Æ £º USART1ÖĞ¶Ï
¹¦    ÄÜ £º232´®¿Ú£¬Êı¾İ×ª·¢£¬ÔëÉù2¡¢LED ÎÊ´ğÊ½
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
*************************************************/
void USART1_IRQHandler(void)//²»¶¨³¤½ÓÊÕ
{
	uint8_t Clear=Clear;//ÕâÖÖ¶¨Òå·½·¨£¬ÓÃÀ´Ïû³ı±àÒëÆ÷µÄ"Ã»ÓĞÓÃµ½"ÌáĞÑ
	uint8_t i = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // Èç¹û½ÓÊÕµ½1¸ö×Ö½Ú
	{ 	
		RxBuf1[Rx1Count++] = USART1->DR;// °Ñ½ÓÊÕµ½µÄ×Ö½Ú±£´æ£¬Êı×éµØÖ·¼Ó1
	} 
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)// Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
	{
		Clear=USART1->SR;// ¶ÁSR¼Ä´æÆ÷
		Clear=USART1->DR;// ¶ÁDR¼Ä´æÆ÷(ÏÈ¶ÁSRÔÙ¶ÁDR£¬¾ÍÊÇÎªÁËÇå³ıIDLEÖĞ¶Ï)
		Rx1Flag=1;// ±ê¼Ç½ÓÊÕµ½ÁË1Ö¡Êı¾İ
		
		if(Rx1Flag==1)//Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
		{
//			Rx1Flag=0;
//			/**/
//			i=0;
//			/**/
//			while(Rx1Count--)// °Ñ½ÓÊÕµ½Êı¾İ·¢ËÍ»Ø´®¿Ú
//			{
//				USART_SendData(USART1, RxBuf1[i++]);	
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//			}
//		 Rx1Count=0;
		}
	}
}
/************************************************
º¯ÊıÃû³Æ £º USART2ÖĞ¶Ï
¹¦    ÄÜ :	485´®¿Ú£¬Êı¾İ×ª·¢£¬ÉäÏß£¨4800baud£©	PM10¡¢ÎÂÊª¶È¡¢ÔëÉù£¬ÎÊ´ğÊ½//memcmp(s1,s2,2);
±È½ÏÊı×éÊÇ·ñÏàÍ¬£¬ÖĞ¼ä¾ùÎª04 00 (Ò»Ìå»¯³ıÍâ),×¢ÒâÆäËüÎ»Ğ£ÑéÎ´Ìí¼Ó£¬Ğ£ÑéÎ»Ê¹ÓÃµÄ·½·¨Ã»Ğ´£¡£¡£¡£¡£¡
»Ø·¢ÏìÓ¦¶ªÊ§Î´Ìí¼Ó
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
±¸	  ×¢ £ºint memcmp(const void *buf1, const void *buf2, unsigned int count);
buf1<buf2,	<0
buf1==buf2,	=0
buf1>buf2,	>0
*************************************************/
void USART2_IRQHandler (void)//DMA²âÊÔÊ§°Ü
{
	uint8_t Clear2=Clear2;//ÕâÖÖ¶¨Òå·½·¨£¬ÓÃÀ´Ïû³ı±àÒëÆ÷µÄ"Ã»ÓĞÓÃµ½"ÌáĞÑ
	uint8_t j = 0;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // Èç¹û½ÓÊÕµ½1¸ö×Ö½Ú
	{ 	
		RxBuf2[Rx2Count++] = USART2->DR;// °Ñ½ÓÊÕµ½µÄ×Ö½Ú±£´æ£¬Êı×éµØÖ·¼Ó1
	} 
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)// Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
	{
		Clear2 = USART2->SR;// ¶ÁSR¼Ä´æÆ÷
		Clear2 = USART2->DR;// ¶ÁDR¼Ä´æÆ÷(ÏÈ¶ÁSRÔÙ¶ÁDR£¬¾ÍÊÇÎªÁËÇå³ıIDLEÖĞ¶Ï)
		Rx2Flag = 1;		// ±ê¼Ç½ÓÊÕµ½ÁË1Ö¡Êı¾İ
	}
}
/************************************************
º¯ÊıÃû³Æ £º USART3ÖĞ¶Ï
¹¦    ÄÜ :	232´®¿Ú£¬Êı¾İ×ª·¢£¬ÃæÏòÖ÷°å,CRC-16/MODBUS       x16+x15+x2+1
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
*************************************************/
void USART3_IRQHandler (void)
{
	uint8_t Clear3=Clear3;//ÕâÖÖ¶¨Òå·½·¨£¬ÓÃÀ´Ïû³ı±àÒëÆ÷µÄ"Ã»ÓĞÓÃµ½"ÌáĞÑ
	uint8_t k = 0;
//	uint8_t tempcount = 0 ;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // Èç¹û½ÓÊÕµ½1¸ö×Ö½Ú
	{ 	
		RxBuf3[Rx3Count++] = USART3->DR;// °Ñ½ÓÊÕµ½µÄ×Ö½Ú±£´æ£¬Êı×éµØÖ·¼Ó1
	} 
	else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)// Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
	{
		
		//Çå³ş¿ÕÏĞ±êÖ¾Î» USART_ClearITPendingBit(USART6, USART_IT_IDLE);
		Clear3 = USART3->SR;// ¶ÁSR¼Ä´æÆ÷
		Clear3 = USART3->DR;// ¶ÁDR¼Ä´æÆ÷(ÏÈ¶ÁSRÔÙ¶ÁDR£¬¾ÍÊÇÎªÁËÇå³ıIDLEÖĞ¶Ï) 
		Rx3Flag = 1;// ±ê¼Ç½ÓÊÕµ½ÁË1Ö¡Êı¾İ
		/**/if(Rx3Flag == 1)//Èç¹û½ÓÊÕµ½1Ö¡Êı
		{
			Rx3Flag=0;
			k=0;
			while(Rx3Count--)// °Ñ½ÓÊÕµ½Êı¾İ·¢ËÍ»Ø´®¿Ú
			{
				USART_SendData(USART3, RxBuf3[k++]);	
				while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			} Rx3Count=0;
		
		
}
			
	}
	
}
/************************************************
º¯ÊıÃû³Æ £º UART4ÖĞ¶Ï
¹¦    ÄÜ :	TTL´®¿Ú£¬Êı¾İ×ª·¢£¬É¢ÉäPM2.5 PM10
²Î    Êı £º 
·µ »Ø Öµ £º 
Ëµ    Ã÷ : PM2.5 PM10 È¡ÖµÌí¼ÓÍê³É£¬´ıÑéÖ¤ 
*************************************************/
void UART4_IRQHandler (void)
 {
	uint8_t Clear4=Clear4;//ÕâÖÖ¶¨Òå·½·¨£¬ÓÃÀ´Ïû³ı±àÒëÆ÷µÄ"Ã»ÓĞÓÃµ½"ÌáĞÑ
	uint8_t o = 0;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) // Èç¹û½ÓÊÕµ½1¸ö×Ö½Ú
	{ 	
		RxBuf4[Rx4Count++] = UART4->DR;// °Ñ½ÓÊÕµ½µÄ×Ö½Ú±£´æ£¬Êı×éµØÖ·¼Ó1
	} 
	else if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)// Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
	{
		Clear4 = UART4->SR;// ¶ÁSR¼Ä´æÆ÷
		Clear4 = UART4->DR;// ¶ÁDR¼Ä´æÆ÷(ÏÈ¶ÁSRÔÙ¶ÁDR£¬¾ÍÊÇÎªÁËÇå³ıIDLEÖĞ¶Ï)
		Rx4Flag = 1;// ±ê¼Ç½ÓÊÕµ½ÁË1Ö¡Êı¾İ
		if(Rx4Flag == 1)//Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
		{
//			Rx4Flag=0;
//			o=0;
//			while(Rx4Count--)// °Ñ½ÓÊÕµ½Êı¾İ·¢ËÍ»Ø´®¿Ú
//			{
//				USART_SendData(UART4, RxBuf4[o++]);	
//				while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
//			}
//			//
//			Rx4Count=0;
		}
	}
}
/************************************************
º¯ÊıÃû³Æ £º UART5ÖĞ¶Ï
¹¦    ÄÜ :	485´®¿ÚRFIDÉ¨¿¨,Êı¾İ³¤¶È18×Ö½Ú
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
*************************************************/
void UART5_IRQHandler (void){	
	uint8_t Clear5=Clear5;//ÕâÖÖ¶¨Òå·½·¨£¬ÓÃÀ´Ïû³ı±àÒëÆ÷µÄ"Ã»ÓĞÓÃµ½"ÌáĞÑ
	uint8_t p = 0;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // Èç¹û½ÓÊÕµ½1¸ö×Ö½Ú
	{ 	
		RxBuf5[Rx5Count++] = UART5->DR;// °Ñ½ÓÊÕµ½µÄ×Ö½Ú±£´æ£¬Êı×éµØÖ·¼Ó1
	} 
	else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)// Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
	{
		Clear5 = UART5->SR;// ¶ÁSR¼Ä´æÆ÷
		Clear5 = UART5->DR;// ¶ÁDR¼Ä´æÆ÷(ÏÈ¶ÁSRÔÙ¶ÁDR£¬¾ÍÊÇÎªÁËÇå³ıIDLEÖĞ¶Ï)
		Rx5Flag = 1;// ±ê¼Ç½ÓÊÕµ½ÁË1Ö¡Êı¾İ
		if(Rx5Flag == 1)//Èç¹û½ÓÊÕµ½1Ö¡Êı¾İ
		{///*	/**/
			Rx5Flag=0;
			p=0;
			TX_MODE_UART5;
			while(Rx5Count--)// °Ñ½ÓÊÕµ½Êı¾İ·¢ËÍ»Ø´®¿Ú
			{
				USART_SendData(UART5, RxBuf5[p++]);	
				while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
			}
			RX_MODE_UART5;
			Rx5Count=0;
		}
	}
}
/************************************************
º¯ÊıÃû³Æ £º DataXhpProcess
¹¦    ÄÜ :	¶ÔÓ¦´®¿Ú2 ,ĞÂ»İÆÕÊı¾İ½ÓÊÕ×ª´æ´¦Àí
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
*************************************************/
void DataXhpProcess(void)
{
	memcpy(&DataXhpRx,RxBuf2,10);			
	Judge = memcmp(DataXhpRx.Header,Asktemp,7);//³¤¶ÈÊÇ×Ö½ÚÊı?¶Ô±ÈÇ°¼¸Î»,»¹ĞèÒªÌí¼ÓĞ£ÑéÎ»ÑéÖ¤
			
	if(Judge==0)
	{
				//
		switch (StateFlag){
		case 1:																			//´ËÊ±ÄÃµ½·çËÙ
		{
			DataTx.WindSpeed = DataXhpRx.Data2;	
			USARTx_SendNByte(USART2,WindDirectAsk,4);	//·¢ËÍ·½ÏòAsk
			Asktemp[0]=WindDirectAsk[0];
			Asktemp[1]=WindDirectAsk[1];	
			StateFlag++;
		}
		break;
		case 2:																			//´ËÊ±ÄÃµ½·çÏò
		{
			DataTx.WindDirection = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,HumidityAsk,4);		//·¢ËÍHumidityAsk
			Asktemp[0]=HumidityAsk[0];
			Asktemp[1]=HumidityAsk[1];	
			StateFlag++;
		}
		break;
		case 3:																			//´ËÊ±ÄÃµ½Humidity
		{
			DataTx.Humidity = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,TempAsk,4);				//·¢ËÍTempAsk
			Asktemp[0]=TempAsk[0];
			Asktemp[1]=TempAsk[1];	
			StateFlag++;
		}
		break;
		case 4:																			//´ËÊ±ÄÃµ½Temp
		{
			DataTx.Temp= DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,kPaAsk,4);				//·¢kPaAsk
			Asktemp[0]=kPaAsk[0];
			Asktemp[1]=kPaAsk[1];	
			StateFlag++;
		}
		break;
		case 5:																			//´ËÊ±ÄÃµ½kPa
		{
			DataTx.kPa = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,Noise1Ask,4);			//·¢ËÍNoise1Ask
			Asktemp[0]=Noise1Ask[0];
			Asktemp[1]=Noise1Ask[1];	
			StateFlag++;
		}
		break;
		case 6:																			//´ËÊ±ÄÃµ½Noise
		{
			DataTx.Noise= DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,WindSpeedAsk,4);	//·¢WindSpeedAsk
			Asktemp[0]=WindSpeedAsk[0];
			Asktemp[1]=WindSpeedAsk[1];	
			StateFlag = 1;
		}
		break;
		default:
						
		break;
		}
	}
	else
	{	
		USARTx_SendNByte(USART2,WindSpeedAsk,4);//ÔÙ´Î·¢ËÍÇëÇó
	}
}


///************************************************
//º¯ÊıÃû³Æ £º AWA5636Process
//¹¦    ÄÜ :	AWA5636ProcessÊı¾İ´¦ÀíÖ÷ÒªÊÔÔëÉù¼ì²â  RS-232
//²Î    Êı £º 
//·µ »Ø Öµ £º 
//×÷    Õß £º 
//*************************************************/
//void AWA5636Process(void)
//{
//	uint16_t AWA5636Check;
//	uint8_t PMchecNum2 = 4;
//	/*memcpy(&DataAWA5636Rx,RxBuf ,30);		//³¤¶È²»È·¶¨
//	//Judge2 = memcmp(DataAWA5636Rx.ProtocolHeader,AWAAsk,4);
//		uint8_t PMcheck = 0;
//		for(;PMchecNum2< ;PMchecNum2++)//Ğ£ÑéºÍÑéÖ¤
//		{
//					AWA5636Check = AWA5636Check + RxBuf [PMchecNum2];
//		}
//		PMchecNum2 = 4;

//		if(AWA5636Check == DataAWA5636Rx.Sumcheck)//
//		{
//		//
//		}


//	*/

//}
/************************************************
º¯ÊıÃû³Æ £º ²¨ÌØÂÊĞŞ¸Ä
¹¦    ÄÜ :	baudtempget ¸ù¾İ¶ÔÓ¦¹¦ÄÜÂëĞŞ¸Äbaud,»¹¿ÉÒÔÌí¼ÓÏàÓ¦Ğ£ÑéÎ»µÄĞŞ¸ÄµÈ
²Î    Êı £º 
·µ »Ø Öµ £º 
×÷    Õß £º 
*************************************************/
uint8_t Baudchange(USART_InitTypeDef USART_InitStructurex,uint8_t baudtempget,USART_TypeDef* USARTx)//USART_TypeDef;
{

	switch (baudtempget){
		case 0x00:
			Baud = 9600;
		break;
		case 0x01:
			Baud = 19200;
		break;
		case 0x02:
			Baud = 115200;
		break;
		case 0x03:
			Baud = 128000;
		break;
		case 0x04:
			Baud = 256000;
		break;
		case 0x05:
			Baud = 110;
		break;
		default:
			Baud = 115200;
		break;
	}
	//USART_InitTypeDef USART_InitStructure2;
	
	USART_InitStructurex.USART_BaudRate = Baud;									//´®¿Ú²¨ÌØÂÊ
	USART_InitStructurex.USART_WordLength = USART_WordLength_8b;						//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructurex.USART_StopBits = USART_StopBits_1;							//Ò»¸öÍ£Ö¹Î»
	USART_InitStructurex.USART_Parity = USART_Parity_No;								//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructurex.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructurex.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
//	USART_InitStructure2.USART_BaudRate = Baud;	
	USART_Init(USARTx, &USART_InitStructurex);			//³õÊ¼»¯´®¿Ú2
	USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);	
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);	 
	USART_Cmd(USARTx, ENABLE);	
	return Baud;
}







/*	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   TX_MODE_USART2;	
		USART_SendData(USART2, TxBuffer2[TxCounter2++]);
	// Write one byte to the transmit data register 
		if(TxCounter2 == NbrOfDataToTransfer2)
		{
	// 		Disable the USART2 Transmit interrupt 
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			RX_MODE_USART2;
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		}
	}
}*/
//485
/*
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_HT6))
    {
        DMA_ClearITPendingBit(DMA1_IT_HT6);//
		
    }

    if(DMA_GetFlagStatus(DMA1_FLAG_TC6))//transfer complete flag.
    {
        DMA_ClearITPendingBit(DMA1_FLAG_TC6);//´¦ÀíÊı¾İÆÚ¼ä¹Ø±Õ±êÖ¾Î»
       //  DMA_ClearITPendingBit(DMA1_IT_GL3);  
        //????????DMA,?????? // 
       // DMA_Cmd(DMA1_Channel3, DISABLE);      
          
        // ????//  
    }
}
*/


void delay_ms(u16 time)       
{      
  u16 i=0;      
  while(time--)       
  {      
    i=12000;      
    while(i--);      
  }      
}

/*
	 if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {   
		// Write one byte to the transmit data register
		USART_SendData(USART2,temp22);
		USART_SEND(USART2,(s8*)temp22,4);
		//Disable the USART2 Transmit interrupt
		osDelay (10000);
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);

    
  }
	
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!= RESET)   //Receive Data register not empty interrupt.
	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE); //æ¸…é™¤ä¸­æ–­æ ‡å¿—.  
		RxBuf5[temp_u5++] = USART_ReceiveData(UART5);// æ•°æ®æ¥æ”¶
		if(temp_u5 == 10)//é•¿åº¦åˆ¤æ–­
		{
			if(RxBuf5[0]==0x68&&RxBuf5[1]==0x51){//
				TX_MODE_UART5;
				osDelay (1000);
				USARTx_SendNByte(UART5,RxBuf5,10);
				while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)//TC=1å‘é€å®Œæˆæ ‡å¿—ä½
				{}
				osDelay (1000);
				temp_u5 = 0;
				RX_MODE_UART5;
			}
			else {
				temp_u5 = 0;
			}		
		}
		//temp_u5 = 0;
	}
	 if (USART_GetITStatus(USART2, USART_IT_TC) == SET)//å‘é€å®Œæˆä¸­æ–­
	 {
		Rx5Flag =1;
	 }
*/

