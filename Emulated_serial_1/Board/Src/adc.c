#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "adc.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

uint16_t AD_Value[10][4]; //ADC转化结果，目标地址
uint16_t After_filter[4]; //存放求平均值之后的结果

///*	还不理解，qiange在w5500.c中� 下面函数初始化放在这里面，在main.c中APPmain	中做了使用,w5500.h中没有使用*/
//__NO_RETURN void ADC_4 (void *argument)
//{
//	osDelay (1000);
//
//	ADC_Initializes();
//	DMA_Configuration();
///*
//	while (1)
//	{
//		W5500_Interrupt_Check();
//
//		W5500_Socket_Set();//W5500端口初始化配置

//		if(W5500_Interrupt)//处理W5500中断
//		{
//			W5500_Interrupt_Process();//W5500中断处理程序框架
//		}
//		if((S0_Data & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
//		{
//			S0_Data&=~S_RECEIVE;
//			Process_Socket_Data(0);//W5500接收并发送接收到的数据
//		}

//		W5500_Send_Delay_Counter++;
//		osDelay (1);
//	}*/
//}
/************************************************
函数名称 ： ADC_GPIO_Configuration
功    能 ： ADC引脚配置
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
说	  明 :  //ADC8： 	PB0
			//ADC9：	PB1
			ADC10： PC0
			ADC11：	PC1
			ADC12： PC2
			ADC13：	PC3
*************************************************/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |  GPIO_Pin_3 ;                         //ADC8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                      //模拟输入 10 11 12 13
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/************************************************
函数名称 ： ADC_Configuration
功    能 ： ADC配置
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    /* ADC1配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 		//设置ADC的工作模式，此处设置为独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                     			//扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;               		 	//ADC工作在连续转化模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//软件触发，关闭外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             		//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 4;                            		//4个通道
    ADC_Init(ADC1, &ADC_InitStructure);

    /******************************************************************
       设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
       使用ADC1,模拟通道x，采样序列号为  ，采样时间 55.5  周期
    *******************************************************************/
    ADC_RegularChannelConfig(ADC1,ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);

// 	开启DMA,需单独配置
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    /* 重置ADC1的校准寄存器 */
    ADC_ResetCalibration(ADC1);
    /* 检查ADC1重置校准寄存器的状态，直到校准寄存器重设完成 */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* 开始ADC1校准 */
    ADC_StartCalibration(ADC1);
    /*检查ADC1校准的结束标志，直到校准完成 */
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/************************************************
函数名称 ： DMA_Configuration
功    能 ： DMA 初始化，我把board.c里面那个关闭掉了
参    数 ： 无
返 回 值 ： 1000倍电压值
作    者 ： strongerHuang
*************************************************/
/* */
void DMA_Configuration(void)//DMA中断未添加
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    DMA_DeInit(DMA1_Channel1); //DMA 通道1寄存器设为缺省//adc1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA 外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //数据传输目的地
    DMA_InitStructure.DMA_BufferSize = 10*4; //DMA缓存大小 采集次数*通道数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA?? x??????
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); //初始化

    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC|DMA_IT_HT,ENABLE); //传输完成和传输过半
    DMA_ClearFlag(DMA1_FLAG_GL1);	// 清除DMA所有标志
    DMA_Cmd(DMA1_Channel1,ENABLE);	//使能通道6

//	DMA_DeInit(DMA1_Channel6);//DMA1通道5配置
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);//指向USART2//外设地址
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA1_CH6_BUF;	//内存地址
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//dma传输方向单向
//	DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;//在board.c中定义//设置DMA在传输时缓冲区的长度
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//设置DMA的外设递增模式，一个外设
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//设置DMA的内存递增模式
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据字长
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//内存数据字长
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//设置DMA的传输模式
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//VeryHigh,High,Medium,Low.	//设置DMA的优先级别
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//关闭内存到内存的DMA模式
//	DMA_Init(DMA1_Channel6,&DMA_InitStructure);
//
//	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC|DMA_IT_HT,ENABLE); //传输完成和传输过半
//	DMA_ClearFlag(DMA1_FLAG_GL6);	// 清除DMA所有标志
//	DMA_Cmd(DMA1_Channel6,ENABLE);	//使能通道6




}
/************************************************
函数名称 ： ADC_Initializes
功    能 ： ADC初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void ADC_Initializes(void)
{
    DMA_Configuration();
    ADC_GPIO_Configuration();
    ADC_Configuration();
}

u16 GetVolt(u16 advalue)
{
    return (u16)(advalue * 330 / 4096); //数据转弧
}

/************************************************
函数名称 ： filter
功    能 ： 滤波，十次取均值
参    数 ： 无
返 回 值 ：
作    者 ：
*************************************************/


void filter(void)
{
    int sum = 0;
    uint8_t  count;
    int i = 0;
    for(i=0; i<4; i++)

    {

        for ( count=0; count<10; count++)

        {
            sum += AD_Value[count][i];
        }

        After_filter[i]=sum/10;

        sum=0;
    }

}






