#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "adc.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

uint16_t AD_Value[10][4]; //ADCת�������Ŀ���ַ
uint16_t After_filter[4]; //�����ƽ��ֵ֮��Ľ��

///*	������⣬qiange��w5500.c�а ���溯����ʼ�����������棬��main.c��APPmain	������ʹ��,w5500.h��û��ʹ��*/
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
//		W5500_Socket_Set();//W5500�˿ڳ�ʼ������

//		if(W5500_Interrupt)//����W5500�ж�
//		{
//			W5500_Interrupt_Process();//W5500�жϴ��������
//		}
//		if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
//		{
//			S0_Data&=~S_RECEIVE;
//			Process_Socket_Data(0);//W5500���ղ����ͽ��յ�������
//		}

//		W5500_Send_Delay_Counter++;
//		osDelay (1);
//	}*/
//}
/************************************************
�������� �� ADC_GPIO_Configuration
��    �� �� ADC��������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
˵	  �� :  //ADC8�� 	PB0
			//ADC9��	PB1
			ADC10�� PC0
			ADC11��	PC1
			ADC12�� PC2
			ADC13��	PC3
*************************************************/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |  GPIO_Pin_3 ;                         //ADC8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                      //ģ������ 10 11 12 13
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/************************************************
�������� �� ADC_Configuration
��    �� �� ADC����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    /* ADC1���� */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 		//����ADC�Ĺ���ģʽ���˴�����Ϊ��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                     			//ɨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;               		 	//ADC����������ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//����������ر��ⲿ����
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             		//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 4;                            		//4��ͨ��
    ADC_Init(ADC1, &ADC_InitStructure);

    /******************************************************************
       ����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
       ʹ��ADC1,ģ��ͨ��x���������к�Ϊ  ������ʱ�� 55.5  ����
    *******************************************************************/
    ADC_RegularChannelConfig(ADC1,ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);

// 	����DMA,�赥������
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    /* ����ADC1��У׼�Ĵ��� */
    ADC_ResetCalibration(ADC1);
    /* ���ADC1����У׼�Ĵ�����״̬��ֱ��У׼�Ĵ���������� */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ��ʼADC1У׼ */
    ADC_StartCalibration(ADC1);
    /*���ADC1У׼�Ľ�����־��ֱ��У׼��� */
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/************************************************
�������� �� DMA_Configuration
��    �� �� DMA ��ʼ�����Ұ�board.c�����Ǹ��رյ���
��    �� �� ��
�� �� ֵ �� 1000����ѹֵ
��    �� �� strongerHuang
*************************************************/
/* */
void DMA_Configuration(void)//DMA�ж�δ���
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    DMA_DeInit(DMA1_Channel1); //DMA ͨ��1�Ĵ�����Ϊȱʡ//adc1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA ����ADC����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //���ݴ���Ŀ�ĵ�
    DMA_InitStructure.DMA_BufferSize = 10*4; //DMA�����С �ɼ�����*ͨ����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA?? x??????
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA���ڴ洫��
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); //��ʼ��

    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC|DMA_IT_HT,ENABLE); //������ɺʹ������
    DMA_ClearFlag(DMA1_FLAG_GL1);	// ���DMA���б�־
    DMA_Cmd(DMA1_Channel1,ENABLE);	//ʹ��ͨ��6

//	DMA_DeInit(DMA1_Channel6);//DMA1ͨ��5����
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);//ָ��USART2//�����ַ
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA1_CH6_BUF;	//�ڴ��ַ
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//dma���䷽����
//	DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;//��board.c�ж���//����DMA�ڴ���ʱ�������ĳ���
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����DMA���������ģʽ��һ������
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//����DMA���ڴ����ģʽ
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���������ֳ�
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�ڴ������ֳ�
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//����DMA�Ĵ���ģʽ
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//VeryHigh,High,Medium,Low.	//����DMA�����ȼ���
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//�ر��ڴ浽�ڴ��DMAģʽ
//	DMA_Init(DMA1_Channel6,&DMA_InitStructure);
//
//	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC|DMA_IT_HT,ENABLE); //������ɺʹ������
//	DMA_ClearFlag(DMA1_FLAG_GL6);	// ���DMA���б�־
//	DMA_Cmd(DMA1_Channel6,ENABLE);	//ʹ��ͨ��6




}
/************************************************
�������� �� ADC_Initializes
��    �� �� ADC��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void ADC_Initializes(void)
{
    DMA_Configuration();
    ADC_GPIO_Configuration();
    ADC_Configuration();
}

u16 GetVolt(u16 advalue)
{
    return (u16)(advalue * 330 / 4096); //����ת��
}

/************************************************
�������� �� filter
��    �� �� �˲���ʮ��ȡ��ֵ
��    �� �� ��
�� �� ֵ ��
��    �� ��
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






