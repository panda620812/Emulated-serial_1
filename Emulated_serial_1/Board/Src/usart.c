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
			UART4 TX:PC10 	RX:PC11 	//TTL//可是是				9600 	PM2.5 PM10
			UART5 TX:PC12 	RX:PD2   1	//485	PB3_RS485_CTR2  	9600 	RFID
			其中对主板连接的还不清楚，可能是
*/
#define   count1 10
extern float temp1;
extern float temp2;
extern float temp3;
extern float temp4;
extern float temp5;


struct DataTxType 			DataTx;				//数据发送（多状态1）
struct DataPMType			DataPm;
struct DataPM25PM10RxType	DataRxPm;			//PM25PM10接收存放
struct DataXhpRxType		DataXhpRx;		//新惠普
struct DataTempPaRxType		DataTempPaRx;	//β射线法接收温度压力
struct DataRFIDRxType		DataRFIDRx;
struct DataRFIDTxType		DataRFIDTx;
struct DataAWA5636RxType	AWA5636Rx;

//struct PNetTopologyRecord pNet;

//uint8_t uinttemp1 = (uint8_t)temp1;
uint8_t temp_u5 = 0;
//接收区缓存
uint8_t RxBuf1[100] = {0x00};
uint8_t RxBuf2[100] = {0x00};
uint8_t RxBuf3[100] = {0x00};
uint8_t RxBuf4[100] = {0x00};
uint8_t RxBuf5[100] = {0x00};
//接收帧提取存储数组
uint8_t RxBuf1temp[24] = {0x00};
uint8_t RxBuf2temp[24] = {0x00};
uint8_t RxBuf3temp[24] = {0x00};
uint8_t RxBuf4temp[24] = {0x00};
uint8_t RxBuf5temp[24] = {0x00};
//发送区缓存
uint8_t TxBuf1[];
uint8_t TxBuf2[10];
uint8_t TxBuf3[];
uint8_t TxBuf4[];
uint8_t TxBuf5[10];
//接收帧标志位
uint8_t Rx1Flag = 0;
uint8_t Rx2Flag = 0;
uint8_t Rx3Flag = 0;
uint8_t Rx4Flag = 0;
uint8_t Rx5Flag = 0;
//鎺ユ敹璁℃暟
uint8_t Rx1Count = 0;
uint8_t Rx2Count = 0;
uint8_t Rx3Count = 0;
uint8_t Rx4Count = 0;
uint8_t Rx5Count = 0;

//Baud
uint32_t Baud  = 115200;
uint32_t Baud1 = 115200;
uint32_t Baud2 = 115200;//9600 温湿度风向  + 噪声
uint32_t Baud3 = 115200;
uint32_t Baud4 = 115200;//9600 PM2.5 PM10
uint32_t Baud5 = 115200;//9600 RFID

uint32_t Baud1temp1;
uint32_t Baud1temp2;
uint8_t  Baud1temp[3] = {0};

//新惠普
//控制码 有一个一体化风速风向，湿度有个土壤湿度,在开机时发送一个请求(未添加)，之后进入循环Ask，但是失去应答怎么解决？
uint8_t WindSpeedAsk[4] 	= {0x01,0x80,0x01,0x80};
uint8_t WindDirectAsk[4] 	= {0x01,0x81,0xC0,0x40};
uint8_t HumidityAsk[4] 		= {0x01,0x82,0x80,0x41};
uint8_t TempAsk[4] 				= {0x01,0x83,0x41,0x81};
uint8_t kPaAsk[4] 				= {0x01,0x84,0x00,0x43};
uint8_t Noise1Ask[4] 			= {0x01,0x99,0xC0,0x4A};
uint8_t Asktemp[4] 				= {0x10,0x80,0x40,00};
uint8_t Judge;				//校验判断
uint8_t	StateFlag = 1;
//AWA5636	噪声
uint8_t AWAAsk[4] =	{'A','W','A','A'};


uint16_t index1=0,index2=0,index3=0,flag1=0,flag2=0,flag3=0;
uint16_t temp110;
uint16_t temp22 = 0;
uint16_t temp10;
uint16_t temp9;

//PM2.5 PM10 值获取
uint16_t PM2_5;
uint32_t PM10;


void delay_ms(u16 time) ;
uint8_t Baudchange(USART_InitTypeDef USART_InitStructurex,uint8_t baudtempget,USART_TypeDef* USARTx);//Baud修改
void DataXhpProcess(void);//新惠普数据处理
void TempPaProcess(void);//β射线法接收温度压力流量部分


//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */

USART_InitTypeDef USART_InitStructure;
USART_InitTypeDef USART_InitStructure1;
USART_InitTypeDef USART_InitStructure2;
USART_InitTypeDef USART_InitStructure3;
USART_InitTypeDef USART_InitStructure4;
USART_InitTypeDef USART_InitStructure5;
/* 在io_retarget里面有
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
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
//重定义fputc函数
 int fputc(int ch, FILE *f)
{
	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕
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
函数名称 ： void USART_Configuration()
功    能 ： 串口初始化
参    数 ： 无
返 回 值 ： 无
说	  明 :	usart3 与 jtag 引脚复用了，所以disable ,usart2 DMA
*************************************************/
void USART_Configuration()//与主板连接的是一个232接口
{
    //结构体初始化
    DataTx.Startflag		=	0xAA;
    DataTx.SendChannel 	=	0x03;//需修改
    DataTx.FunctionCode 	= 	0x00;//
    DataTx.DataLengh		=	0x12;//因对齐方式需确认


    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO 	|
                            RCC_APB2Periph_USART1	|
                            RCC_APB2Periph_GPIOA	|
                            RCC_APB2Periph_GPIOB 	|
                            RCC_APB2Periph_GPIOC	|
                            RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIO时钟
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART2	|
                            RCC_APB1Periph_USART3	|
                            RCC_APB1Periph_UART4	|
                            RCC_APB1Periph_UART5,ENABLE);	//使能USART
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//CRC时钟使能

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使用SW禁用JTAG*/


    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10
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

    // 串口3重映射
    //GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);//
    /******************************************************************
     USART参数初始化:  波特率     传输位数   停止位数  校验位数
                      115200         8          1      0(NO)
    *******************************************************************/
    USART_InitStructure1.USART_BaudRate = Baud1;									//串口波特率
    USART_InitStructure1.USART_WordLength = USART_WordLength_8b;					//字长为8位数据格式
    USART_InitStructure1.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructure1.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructure1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructure1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
    //USART_BRR
    USART_Init(USART1, &USART_InitStructure1); 			//初始化串口1
    USART_Cmd(USART1, ENABLE);                    		//使能串口1

    USART_InitStructure2.USART_BaudRate = Baud2;									//串口波特率
    USART_InitStructure2.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
    USART_InitStructure2.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructure2.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructure2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructure2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

    USART_Init(USART2, &USART_InitStructure2);			//初始化串口2
    USART_Cmd(USART2, ENABLE);							//使能串口2

    USART_InitStructure3.USART_BaudRate = Baud3;									//串口波特率
    USART_InitStructure3.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
    USART_InitStructure3.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructure3.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructure3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructure3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);		//采用DMA方式接收
//	DMA_Cmd(DMA1_Channel6, ENABLE);

    USART_Init(USART3, &USART_InitStructure3); 			//初始化串口3
    USART_Cmd(USART3, ENABLE);                    		//使能串口3
    //USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);		//采用DMA方式接收

    USART_InitStructure4.USART_BaudRate = Baud4;									//串口波特率
    USART_InitStructure4.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
    USART_InitStructure4.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructure4.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructure4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructure4.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure4); 			//初始化串口4
    USART_Cmd(UART4, ENABLE);                    		//使能串口4

    USART_InitStructure5.USART_BaudRate = Baud5;									//串口波特率
    USART_InitStructure5.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
    USART_InitStructure5.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructure5.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructure5.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructure5.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &USART_InitStructure5); 			//初始化串口5
    USART_Cmd(UART5, ENABLE);                    		//使能串口5


//	TX_MODE_USART2;
//	TX_MODE_UART5;
    //接收中断使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    /* 打开空闲中断 */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
    /* 使能串口1 */
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_Cmd(UART4, ENABLE);
    USART_Cmd(UART5, ENABLE);

    USART_ClearFlag(USART1, USART_FLAG_TC); //发送标志位清零
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
//TXE=发送数据寄存器空,TC=发送结束
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
函数名称 ： USARTx_SendChar
功    能 ： 串口 x 发送一个字符
参    数 ： Data --- 数据
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void USARTx_SendByte(USART_TypeDef* USARTx	,	uint8_t Data)
{
    while((USARTx->SR & USART_FLAG_TXE) == RESET);
    USARTx->DR = (Data & (uint16_t)0x01FF);//只有低9位有效，主要防止干扰
}

/************************************************
函数名称 ： USARTx_SendNByte
功    能 ： 串口x发送N个字符
参    数 ： pData ----- 字符串
            Length --- 长度
返 回 值 ： 无
作    者 ： strongerHuang
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
函数名称 ： USART1中断
功    能 ：232串口，数据转发，噪声2、LED 问答式
参    数 ：
返 回 值 ：
作    者 ：
*************************************************/
void USART1_IRQHandler(void)//不定长接收
{
    uint8_t Clear=Clear;//这种定义方法，用来消除编译器的"没有用到"提醒
    uint8_t i = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 如果接收到1个字节
    {
        RxBuf1[Rx1Count++] = USART1->DR;// 把接收到的字节保存，数组地址加1
    }
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
    {
        Clear=USART1->SR;// 读SR寄存器
        Clear=USART1->DR;// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
        Rx1Flag=1;// 标记接收到了1帧数据

        if(Rx1Flag==1)//如果接收到1帧数据
        {
//			Rx1Flag=0;
//			/**/
//			i=0;
//			/**/
//			while(Rx1Count--)// 把接收到数据发送回串口
//			{
//				USART_SendData(USART1, RxBuf1[i++]);
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//			}
//		 Rx1Count=0;
        }
    }
}
/************************************************
函数名称 ： USART2中断
功    能 :	485串口，数据转发，射线（4800baud）	PM10、温湿度、噪声，问答式//memcmp(s1,s2,2);
比较数组是否相同，中间均为04 00 (一体化除外),注意其它位校验未添加，校验位使用的方法没写！！！！！
回发响应丢失未添加
参    数 ：
返 回 值 ：
作    者 ：
备	  注 ：int memcmp(const void *buf1, const void *buf2, unsigned int count);
buf1<buf2,	<0
buf1==buf2,	=0
buf1>buf2,	>0
*************************************************/
void USART2_IRQHandler (void)//DMA测试失败
{
    uint8_t Clear2=Clear2;//这种定义方法，用来消除编译器的"没有用到"提醒
    uint8_t j = 0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 如果接收到1个字节
    {
        RxBuf2[Rx2Count++] = USART2->DR;// 把接收到的字节保存，数组地址加1
    }
    else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
    {
        Clear2 = USART2->SR;// 读SR寄存器
        Clear2 = USART2->DR;// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
        Rx2Flag = 1;		// 标记接收到了1帧数据
    }
}
/************************************************
函数名称 ： USART3中断
功    能 :	232串口，数据转发，面向主板,CRC-16/MODBUS       x16+x15+x2+1
参    数 ：
返 回 值 ：
作    者 ：
*************************************************/
void USART3_IRQHandler (void)
{
    uint8_t Clear3=Clear3;//这种定义方法，用来消除编译器的"没有用到"提醒
    uint8_t k = 0;
//	uint8_t tempcount = 0 ;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 如果接收到1个字节
    {
        RxBuf3[Rx3Count++] = USART3->DR;// 把接收到的字节保存，数组地址加1
    }
    else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
    {

        //清楚空闲标志位 USART_ClearITPendingBit(USART6, USART_IT_IDLE);
        Clear3 = USART3->SR;// 读SR寄存器
        Clear3 = USART3->DR;// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
        Rx3Flag = 1;// 标记接收到了1帧数据
        /**/if(Rx3Flag == 1)//如果接收到1帧数
        {
            Rx3Flag=0;
            k=0;
            while(Rx3Count--)// 把接收到数据发送回串口
            {
                USART_SendData(USART3, RxBuf3[k++]);
                while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
            }
            Rx3Count=0;


        }

    }

}
/************************************************
函数名称 ： UART4中断
功    能 :	TTL串口，数据转发，散射PM2.5 PM10
参    数 ：
返 回 值 ：
说    明 : PM2.5 PM10 取值添加完成，待验证
*************************************************/
void UART4_IRQHandler (void)
{
    uint8_t Clear4=Clear4;//这种定义方法，用来消除编译器的"没有用到"提醒
    uint8_t o = 0;

    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) // 如果接收到1个字节
    {
        RxBuf4[Rx4Count++] = UART4->DR;// 把接收到的字节保存，数组地址加1
    }
    else if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
    {
        Clear4 = UART4->SR;// 读SR寄存器
        Clear4 = UART4->DR;// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
        Rx4Flag = 1;// 标记接收到了1帧数据
        if(Rx4Flag == 1)//如果接收到1帧数据
        {
//			Rx4Flag=0;
//			o=0;
//			while(Rx4Count--)// 把接收到数据发送回串口
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
函数名称 ： UART5中断
功    能 :	485串口RFID扫卡,数据长度18字节
参    数 ：
返 回 值 ：
作    者 ：
*************************************************/
void UART5_IRQHandler (void) {
    uint8_t Clear5=Clear5;//这种定义方法，用来消除编译器的"没有用到"提醒
    uint8_t p = 0;

    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // 如果接收到1个字节
    {
        RxBuf5[Rx5Count++] = UART5->DR;// 把接收到的字节保存，数组地址加1
    }
    else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
    {
        Clear5 = UART5->SR;// 读SR寄存器
        Clear5 = UART5->DR;// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
        Rx5Flag = 1;// 标记接收到了1帧数据
        if(Rx5Flag == 1)//如果接收到1帧数据
        {   ///*	/**/
            Rx5Flag=0;
            p=0;
            TX_MODE_UART5;
            while(Rx5Count--)// 把接收到数据发送回串口
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
函数名称 ： DataXhpProcess
功    能 :	对应串口2 ,新惠普数据接收转存处理
参    数 ：
返 回 值 ：
作    者 ：
*************************************************/
void DataXhpProcess(void)
{
    memcpy(&DataXhpRx,RxBuf2,10);
    Judge = memcmp(DataXhpRx.Header,Asktemp,7);//长度是字节数?对比前几位,还需要添加校验位验证

    if(Judge==0)
    {
        //
        switch (StateFlag) {
        case 1:																			//此时拿到风速
        {
            DataTx.WindSpeed = DataXhpRx.Data2;
            USARTx_SendNByte(USART2,WindDirectAsk,4);	//发送方向Ask
            Asktemp[0]=WindDirectAsk[0];
            Asktemp[1]=WindDirectAsk[1];
            StateFlag++;
        }
        break;
        case 2:																			//此时拿到风向
        {
            DataTx.WindDirection = DataXhpRx.Data3;
            USARTx_SendNByte(USART2,HumidityAsk,4);		//发送HumidityAsk
            Asktemp[0]=HumidityAsk[0];
            Asktemp[1]=HumidityAsk[1];
            StateFlag++;
        }
        break;
        case 3:																			//此时拿到Humidity
        {
            DataTx.Humidity = DataXhpRx.Data3;
            USARTx_SendNByte(USART2,TempAsk,4);				//发送TempAsk
            Asktemp[0]=TempAsk[0];
            Asktemp[1]=TempAsk[1];
            StateFlag++;
        }
        break;
        case 4:																			//此时拿到Temp
        {
            DataTx.Temp= DataXhpRx.Data3;
            USARTx_SendNByte(USART2,kPaAsk,4);				//发kPaAsk
            Asktemp[0]=kPaAsk[0];
            Asktemp[1]=kPaAsk[1];
            StateFlag++;
        }
        break;
        case 5:																			//此时拿到kPa
        {
            DataTx.kPa = DataXhpRx.Data3;
            USARTx_SendNByte(USART2,Noise1Ask,4);			//发送Noise1Ask
            Asktemp[0]=Noise1Ask[0];
            Asktemp[1]=Noise1Ask[1];
            StateFlag++;
        }
        break;
        case 6:																			//此时拿到Noise
        {
            DataTx.Noise= DataXhpRx.Data3;
            USARTx_SendNByte(USART2,WindSpeedAsk,4);	//发WindSpeedAsk
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
        USARTx_SendNByte(USART2,WindSpeedAsk,4);//再次发送请求
    }
}


///************************************************
//函数名称 ： AWA5636Process
//功    能 :	AWA5636Process数据处理主要试噪声检测  RS-232
//参    数 ：
//返 回 值 ：
//作    者 ：
//*************************************************/
//void AWA5636Process(void)
//{
//	uint16_t AWA5636Check;
//	uint8_t PMchecNum2 = 4;
//	/*memcpy(&DataAWA5636Rx,RxBuf ,30);		//长度不确定
//	//Judge2 = memcmp(DataAWA5636Rx.ProtocolHeader,AWAAsk,4);
//		uint8_t PMcheck = 0;
//		for(;PMchecNum2< ;PMchecNum2++)//校验和验证
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
函数名称 ： 波特率修改
功    能 :	baudtempget 根据对应功能码修改baud,还可以添加相应校验位的修改等
参    数 ：
返 回 值 ：
作    者 ：
*************************************************/
uint8_t Baudchange(USART_InitTypeDef USART_InitStructurex,uint8_t baudtempget,USART_TypeDef* USARTx)//USART_TypeDef;
{

    switch (baudtempget) {
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

    USART_InitStructurex.USART_BaudRate = Baud;									//串口波特率
    USART_InitStructurex.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
    USART_InitStructurex.USART_StopBits = USART_StopBits_1;							//一个停止位
    USART_InitStructurex.USART_Parity = USART_Parity_No;								//无奇偶校验位
    USART_InitStructurex.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
    USART_InitStructurex.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//	USART_InitStructure2.USART_BaudRate = Baud;
    USART_Init(USARTx, &USART_InitStructurex);			//初始化串口2
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
        DMA_ClearITPendingBit(DMA1_FLAG_TC6);//处理数据期间关闭标志位
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
		USART_ClearITPendingBit(UART5,USART_IT_RXNE); //娓呴櫎涓柇鏍囧織.
		RxBuf5[temp_u5++] = USART_ReceiveData(UART5);// 鏁版嵁鎺ユ敹
		if(temp_u5 == 10)//闀垮害鍒ゆ柇
		{
			if(RxBuf5[0]==0x68&&RxBuf5[1]==0x51){//
				TX_MODE_UART5;
				osDelay (1000);
				USARTx_SendNByte(UART5,RxBuf5,10);
				while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)//TC=1鍙戦�佸畬鎴愭爣蹇椾綅
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
	 if (USART_GetITStatus(USART2, USART_IT_TC) == SET)//鍙戦�佸畬鎴愪腑鏂�
	 {
		Rx5Flag =1;
	 }
*/

