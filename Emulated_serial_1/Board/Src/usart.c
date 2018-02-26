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
			UART4 TX:PC10 	RX:PC11 	//TTL//������				9600 	PM2.5 PM10
			UART5 TX:PC12 	RX:PD2   1	//485	PB3_RS485_CTR2  	9600 	RFID
			���ж��������ӵĻ��������������
*/
#define   count1 10
extern float temp1;
extern float temp2;
extern float temp3;
extern float temp4;
extern float temp5;


struct DataTxType 			DataTx;				//���ݷ��ͣ���״̬1��
struct DataPMType			DataPm;
struct DataPM25PM10RxType	DataRxPm;			//PM25PM10���մ��
struct DataXhpRxType		DataXhpRx;		//�»��� 
struct DataTempPaRxType		DataTempPaRx;	//�����߷������¶�ѹ��
struct DataRFIDRxType		DataRFIDRx;
struct DataRFIDTxType		DataRFIDTx;
struct DataAWA5636RxType	AWA5636Rx;

//struct PNetTopologyRecord pNet;

//uint8_t uinttemp1 = (uint8_t)temp1;
uint8_t temp_u5 = 0;
//����������
uint8_t RxBuf1[100] = {0x00};
uint8_t RxBuf2[100] = {0x00};
uint8_t RxBuf3[100] = {0x00};
uint8_t RxBuf4[100] = {0x00};
uint8_t RxBuf5[100] = {0x00};
//����֡��ȡ�洢����
uint8_t RxBuf1temp[24] = {0x00};
uint8_t RxBuf2temp[24] = {0x00};
uint8_t RxBuf3temp[24] = {0x00};
uint8_t RxBuf4temp[24] = {0x00};
uint8_t RxBuf5temp[24] = {0x00};
//����������
uint8_t TxBuf1[];
uint8_t TxBuf2[10];
uint8_t TxBuf3[];
uint8_t TxBuf4[];
uint8_t TxBuf5[10];
//����֡��־λ
uint8_t Rx1Flag = 0;
uint8_t Rx2Flag = 0;
uint8_t Rx3Flag = 0;
uint8_t Rx4Flag = 0;
uint8_t Rx5Flag = 0;
//接收计数
uint8_t Rx1Count = 0;
uint8_t Rx2Count = 0;
uint8_t Rx3Count = 0;
uint8_t Rx4Count = 0;
uint8_t Rx5Count = 0;

//Baud
uint32_t Baud  = 115200;
uint32_t Baud1 = 115200;
uint32_t Baud2 = 115200;//9600 ��ʪ�ȷ���  + ����
uint32_t Baud3 = 115200;
uint32_t Baud4 = 115200;//9600 PM2.5 PM10	
uint32_t Baud5 = 115200;//9600 RFID

uint32_t Baud1temp1;
uint32_t Baud1temp2;
uint8_t  Baud1temp[3] = {0};

//�»���
//������ ��һ��һ�廯���ٷ���ʪ���и�����ʪ��,�ڿ���ʱ����һ������(δ���)��֮�����ѭ��Ask������ʧȥӦ����ô�����
uint8_t WindSpeedAsk[4] 	= {0x01,0x80,0x01,0x80};
uint8_t WindDirectAsk[4] 	= {0x01,0x81,0xC0,0x40};	
uint8_t HumidityAsk[4] 		= {0x01,0x82,0x80,0x41};	
uint8_t TempAsk[4] 				= {0x01,0x83,0x41,0x81};	
uint8_t kPaAsk[4] 				= {0x01,0x84,0x00,0x43};	
uint8_t Noise1Ask[4] 			= {0x01,0x99,0xC0,0x4A};
uint8_t Asktemp[4] 				= {0x10,0x80,0x40,00};
uint8_t Judge;				//У���ж�
uint8_t	StateFlag = 1;
//AWA5636	����
uint8_t AWAAsk[4] =	{'A','W','A','A'};


uint16_t index1=0,index2=0,index3=0,flag1=0,flag2=0,flag3=0;
uint16_t temp110;
uint16_t temp22 = 0;
uint16_t temp10;
uint16_t temp9;

//PM2.5 PM10 ֵ��ȡ
uint16_t PM2_5;
uint32_t PM10;


void delay_ms(u16 time) ;
uint8_t Baudchange(USART_InitTypeDef USART_InitStructurex,uint8_t baudtempget,USART_TypeDef* USARTx);//Baud�޸�
void DataXhpProcess(void);//�»������ݴ���
void TempPaProcess(void);//�����߷������¶�ѹ����������


//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */

	USART_InitTypeDef USART_InitStructure;
	USART_InitTypeDef USART_InitStructure1;
	USART_InitTypeDef USART_InitStructure2;
	USART_InitTypeDef USART_InitStructure3;
	USART_InitTypeDef USART_InitStructure4;	
	USART_InitTypeDef USART_InitStructure5;
/* ��io_retarget������
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
 int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//ѭ������,ֱ���������   
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
�������� �� void USART_Configuration()
��    �� �� ���ڳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
˵	  �� :	usart3 �� jtag ���Ÿ����ˣ�����disable ,usart2 DMA
*************************************************/
void USART_Configuration()//���������ӵ���һ��232�ӿ�
{
	//�ṹ���ʼ��
	DataTx.Startflag		=	0xAA;
	DataTx.SendChannel 	=	0x03;//���޸�
	DataTx.FunctionCode 	= 	0x00;//
	DataTx.DataLengh		=	0x12;//����뷽ʽ��ȷ��
	
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO 	| 
							RCC_APB2Periph_USART1	|
							RCC_APB2Periph_GPIOA	|
							RCC_APB2Periph_GPIOB 	| 
							RCC_APB2Periph_GPIOC	| 
							RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��GPIOʱ��
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART2	|
							RCC_APB1Periph_USART3	|
							RCC_APB1Periph_UART4	|	
							RCC_APB1Periph_UART5,ENABLE);	//ʹ��USART
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//CRCʱ��ʹ��
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SW����JTAG*/
	
	
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
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
	
	// ����3��ӳ��
	//GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);//
  /******************************************************************
   USART������ʼ��:  ������     ����λ��   ֹͣλ��  У��λ��
                    115200         8          1      0(NO)
  *******************************************************************/
	USART_InitStructure1.USART_BaudRate = Baud1;									//���ڲ�����
	USART_InitStructure1.USART_WordLength = USART_WordLength_8b;					//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure1.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure1.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	//USART_BRR
	USART_Init(USART1, &USART_InitStructure1); 			//��ʼ������1
	USART_Cmd(USART1, ENABLE);                    		//ʹ�ܴ���1 
	
	USART_InitStructure2.USART_BaudRate = Baud2;									//���ڲ�����
	USART_InitStructure2.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure2.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure2.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ	
	
	USART_Init(USART2, &USART_InitStructure2);			//��ʼ������2
	USART_Cmd(USART2, ENABLE);							//ʹ�ܴ���2 
	
	USART_InitStructure3.USART_BaudRate = Baud3;									//���ڲ�����
	USART_InitStructure3.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure3.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure3.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
//  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);		//����DMA��ʽ���� 
//	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	USART_Init(USART3, &USART_InitStructure3); 			//��ʼ������3
	USART_Cmd(USART3, ENABLE);                    		//ʹ�ܴ���3
	//USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);		//����DMA��ʽ���� 
	
	USART_InitStructure4.USART_BaudRate = Baud4;									//���ڲ�����
	USART_InitStructure4.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure4.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure4.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure4.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(UART4, &USART_InitStructure4); 			//��ʼ������4
	USART_Cmd(UART4, ENABLE);                    		//ʹ�ܴ���4
	
	USART_InitStructure5.USART_BaudRate = Baud5;									//���ڲ�����
	USART_InitStructure5.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure5.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure5.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure5.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure5.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(UART5, &USART_InitStructure5); 			//��ʼ������5
	USART_Cmd(UART5, ENABLE);                    		//ʹ�ܴ���5

	
//	TX_MODE_USART2;
//	TX_MODE_UART5;
	//�����ж�ʹ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	  
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	/* �򿪿����ж� */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	/* ʹ�ܴ���1 */
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);	
	USART_Cmd(UART4, ENABLE);	
	USART_Cmd(UART5, ENABLE);	
	
	USART_ClearFlag(USART1, USART_FLAG_TC); //���ͱ�־λ����
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
//TXE=�������ݼĴ�����,TC=���ͽ���
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
�������� �� USARTx_SendChar
��    �� �� ���� x ����һ���ַ�
��    �� �� Data --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void USARTx_SendByte(USART_TypeDef* USARTx	,	uint8_t Data)
{
  while((USARTx->SR & USART_FLAG_TXE) == RESET);
  USARTx->DR = (Data & (uint16_t)0x01FF);//ֻ�е�9λ��Ч����Ҫ��ֹ����
}

/************************************************
�������� �� USARTx_SendNByte
��    �� �� ����x����N���ַ�
��    �� �� pData ----- �ַ���
            Length --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
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
�������� �� USART1�ж�
��    �� ��232���ڣ�����ת��������2��LED �ʴ�ʽ
��    �� �� 
�� �� ֵ �� 
��    �� �� 
*************************************************/
void USART1_IRQHandler(void)//����������
{
	uint8_t Clear=Clear;//���ֶ��巽��������������������"û���õ�"����
	uint8_t i = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		RxBuf1[Rx1Count++] = USART1->DR;// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		Clear=USART1->SR;// ��SR�Ĵ���
		Clear=USART1->DR;// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�)
		Rx1Flag=1;// ��ǽ��յ���1֡����
		
		if(Rx1Flag==1)//������յ�1֡����
		{
//			Rx1Flag=0;
//			/**/
//			i=0;
//			/**/
//			while(Rx1Count--)// �ѽ��յ����ݷ��ͻش���
//			{
//				USART_SendData(USART1, RxBuf1[i++]);	
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//			}
//		 Rx1Count=0;
		}
	}
}
/************************************************
�������� �� USART2�ж�
��    �� :	485���ڣ�����ת�������ߣ�4800baud��	PM10����ʪ�ȡ��������ʴ�ʽ//memcmp(s1,s2,2);
�Ƚ������Ƿ���ͬ���м��Ϊ04 00 (һ�廯����),ע������λУ��δ��ӣ�У��λʹ�õķ���ûд����������
�ط���Ӧ��ʧδ���
��    �� �� 
�� �� ֵ �� 
��    �� �� 
��	  ע ��int memcmp(const void *buf1, const void *buf2, unsigned int count);
buf1<buf2,	<0
buf1==buf2,	=0
buf1>buf2,	>0
*************************************************/
void USART2_IRQHandler (void)//DMA����ʧ��
{
	uint8_t Clear2=Clear2;//���ֶ��巽��������������������"û���õ�"����
	uint8_t j = 0;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		RxBuf2[Rx2Count++] = USART2->DR;// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		Clear2 = USART2->SR;// ��SR�Ĵ���
		Clear2 = USART2->DR;// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�)
		Rx2Flag = 1;		// ��ǽ��յ���1֡����
	}
}
/************************************************
�������� �� USART3�ж�
��    �� :	232���ڣ�����ת������������,CRC-16/MODBUS       x16+x15+x2+1
��    �� �� 
�� �� ֵ �� 
��    �� �� 
*************************************************/
void USART3_IRQHandler (void)
{
	uint8_t Clear3=Clear3;//���ֶ��巽��������������������"û���õ�"����
	uint8_t k = 0;
//	uint8_t tempcount = 0 ;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		RxBuf3[Rx3Count++] = USART3->DR;// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		
		//������б�־λ USART_ClearITPendingBit(USART6, USART_IT_IDLE);
		Clear3 = USART3->SR;// ��SR�Ĵ���
		Clear3 = USART3->DR;// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�) 
		Rx3Flag = 1;// ��ǽ��յ���1֡����
		/**/if(Rx3Flag == 1)//������յ�1֡��
		{
			Rx3Flag=0;
			k=0;
			while(Rx3Count--)// �ѽ��յ����ݷ��ͻش���
			{
				USART_SendData(USART3, RxBuf3[k++]);	
				while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			} Rx3Count=0;
		
		
}
			
	}
	
}
/************************************************
�������� �� UART4�ж�
��    �� :	TTL���ڣ�����ת����ɢ��PM2.5 PM10
��    �� �� 
�� �� ֵ �� 
˵    �� : PM2.5 PM10 ȡֵ�����ɣ�����֤ 
*************************************************/
void UART4_IRQHandler (void)
 {
	uint8_t Clear4=Clear4;//���ֶ��巽��������������������"û���õ�"����
	uint8_t o = 0;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		RxBuf4[Rx4Count++] = UART4->DR;// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		Clear4 = UART4->SR;// ��SR�Ĵ���
		Clear4 = UART4->DR;// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�)
		Rx4Flag = 1;// ��ǽ��յ���1֡����
		if(Rx4Flag == 1)//������յ�1֡����
		{
//			Rx4Flag=0;
//			o=0;
//			while(Rx4Count--)// �ѽ��յ����ݷ��ͻش���
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
�������� �� UART5�ж�
��    �� :	485����RFIDɨ��,���ݳ���18�ֽ�
��    �� �� 
�� �� ֵ �� 
��    �� �� 
*************************************************/
void UART5_IRQHandler (void){	
	uint8_t Clear5=Clear5;//���ֶ��巽��������������������"û���õ�"����
	uint8_t p = 0;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		RxBuf5[Rx5Count++] = UART5->DR;// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		Clear5 = UART5->SR;// ��SR�Ĵ���
		Clear5 = UART5->DR;// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�)
		Rx5Flag = 1;// ��ǽ��յ���1֡����
		if(Rx5Flag == 1)//������յ�1֡����
		{///*	/**/
			Rx5Flag=0;
			p=0;
			TX_MODE_UART5;
			while(Rx5Count--)// �ѽ��յ����ݷ��ͻش���
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
�������� �� DataXhpProcess
��    �� :	��Ӧ����2 ,�»������ݽ���ת�洦��
��    �� �� 
�� �� ֵ �� 
��    �� �� 
*************************************************/
void DataXhpProcess(void)
{
	memcpy(&DataXhpRx,RxBuf2,10);			
	Judge = memcmp(DataXhpRx.Header,Asktemp,7);//�������ֽ���?�Ա�ǰ��λ,����Ҫ���У��λ��֤
			
	if(Judge==0)
	{
				//
		switch (StateFlag){
		case 1:																			//��ʱ�õ�����
		{
			DataTx.WindSpeed = DataXhpRx.Data2;	
			USARTx_SendNByte(USART2,WindDirectAsk,4);	//���ͷ���Ask
			Asktemp[0]=WindDirectAsk[0];
			Asktemp[1]=WindDirectAsk[1];	
			StateFlag++;
		}
		break;
		case 2:																			//��ʱ�õ�����
		{
			DataTx.WindDirection = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,HumidityAsk,4);		//����HumidityAsk
			Asktemp[0]=HumidityAsk[0];
			Asktemp[1]=HumidityAsk[1];	
			StateFlag++;
		}
		break;
		case 3:																			//��ʱ�õ�Humidity
		{
			DataTx.Humidity = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,TempAsk,4);				//����TempAsk
			Asktemp[0]=TempAsk[0];
			Asktemp[1]=TempAsk[1];	
			StateFlag++;
		}
		break;
		case 4:																			//��ʱ�õ�Temp
		{
			DataTx.Temp= DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,kPaAsk,4);				//��kPaAsk
			Asktemp[0]=kPaAsk[0];
			Asktemp[1]=kPaAsk[1];	
			StateFlag++;
		}
		break;
		case 5:																			//��ʱ�õ�kPa
		{
			DataTx.kPa = DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,Noise1Ask,4);			//����Noise1Ask
			Asktemp[0]=Noise1Ask[0];
			Asktemp[1]=Noise1Ask[1];	
			StateFlag++;
		}
		break;
		case 6:																			//��ʱ�õ�Noise
		{
			DataTx.Noise= DataXhpRx.Data3;	
			USARTx_SendNByte(USART2,WindSpeedAsk,4);	//��WindSpeedAsk
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
		USARTx_SendNByte(USART2,WindSpeedAsk,4);//�ٴη�������
	}
}


///************************************************
//�������� �� AWA5636Process
//��    �� :	AWA5636Process���ݴ�����Ҫ���������  RS-232
//��    �� �� 
//�� �� ֵ �� 
//��    �� �� 
//*************************************************/
//void AWA5636Process(void)
//{
//	uint16_t AWA5636Check;
//	uint8_t PMchecNum2 = 4;
//	/*memcpy(&DataAWA5636Rx,RxBuf ,30);		//���Ȳ�ȷ��
//	//Judge2 = memcmp(DataAWA5636Rx.ProtocolHeader,AWAAsk,4);
//		uint8_t PMcheck = 0;
//		for(;PMchecNum2< ;PMchecNum2++)//У�����֤
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
�������� �� �������޸�
��    �� :	baudtempget ���ݶ�Ӧ�������޸�baud,�����������ӦУ��λ���޸ĵ�
��    �� �� 
�� �� ֵ �� 
��    �� �� 
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
	
	USART_InitStructurex.USART_BaudRate = Baud;									//���ڲ�����
	USART_InitStructurex.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructurex.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructurex.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructurex.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructurex.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
//	USART_InitStructure2.USART_BaudRate = Baud;	
	USART_Init(USARTx, &USART_InitStructurex);			//��ʼ������2
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
        DMA_ClearITPendingBit(DMA1_FLAG_TC6);//���������ڼ�رձ�־λ
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
		USART_ClearITPendingBit(UART5,USART_IT_RXNE); //清除中断标志.  
		RxBuf5[temp_u5++] = USART_ReceiveData(UART5);// 数据接收
		if(temp_u5 == 10)//长度判断
		{
			if(RxBuf5[0]==0x68&&RxBuf5[1]==0x51){//
				TX_MODE_UART5;
				osDelay (1000);
				USARTx_SendNByte(UART5,RxBuf5,10);
				while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)//TC=1发送完成标志位
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
	 if (USART_GetITStatus(USART2, USART_IT_TC) == SET)//发送完成中断
	 {
		Rx5Flag =1;
	 }
*/

