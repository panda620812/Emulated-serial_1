#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "adc.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

uint16_t AD_Value[10][4]; //ADC×ª»¯½á¹û£¬Ä¿±êµØÖ·
uint16_t After_filter[4]; //´æ·ÅÇóÆ½¾ùÖµÖ®ºóµÄ½á¹û

///*	»¹²»Àí½â£¬qiangeÔÚw5500.cÖĞ° ÏÂÃæº¯Êı³õÊ¼»¯·ÅÔÚÕâÀïÃæ£¬ÔÚmain.cÖĞAPPmain	ÖĞ×öÁËÊ¹ÓÃ,w5500.hÖĞÃ»ÓĞÊ¹ÓÃ*/
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
//		W5500_Socket_Set();//W5500¶Ë¿Ú³õÊ¼»¯ÅäÖÃ

//		if(W5500_Interrupt)//´¦ÀíW5500ÖĞ¶Ï
//		{
//			W5500_Interrupt_Process();//W5500ÖĞ¶Ï´¦Àí³ÌĞò¿ò¼Ü
//		}
//		if((S0_Data & S_RECEIVE) == S_RECEIVE)//Èç¹ûSocket0½ÓÊÕµ½Êı¾İ
//		{
//			S0_Data&=~S_RECEIVE;
//			Process_Socket_Data(0);//W5500½ÓÊÕ²¢·¢ËÍ½ÓÊÕµ½µÄÊı¾İ
//		}

//		W5500_Send_Delay_Counter++;
//		osDelay (1);
//	}*/
//}
/************************************************
º¯ÊıÃû³Æ £º ADC_GPIO_Configuration
¹¦    ÄÜ £º ADCÒı½ÅÅäÖÃ
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º ÎŞ
×÷    Õß £º strongerHuang
Ëµ	  Ã÷ :  //ADC8£º 	PB0
			//ADC9£º	PB1
			ADC10£º PC0
			ADC11£º	PC1
			ADC12£º PC2
			ADC13£º	PC3
*************************************************/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |  GPIO_Pin_3 ;                         //ADC8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                      //Ä£ÄâÊäÈë 10 11 12 13
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/************************************************
º¯ÊıÃû³Æ £º ADC_Configuration
¹¦    ÄÜ £º ADCÅäÖÃ
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º ÎŞ
×÷    Õß £º strongerHuang
*************************************************/
void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    /* ADC1ÅäÖÃ */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 		//ÉèÖÃADCµÄ¹¤×÷Ä£Ê½£¬´Ë´¦ÉèÖÃÎª¶ÀÁ¢¹¤×÷Ä£Ê½
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                     			//É¨ÃèÄ£Ê½
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;               		 	//ADC¹¤×÷ÔÚÁ¬Ğø×ª»¯Ä£Ê½
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//Èí¼ş´¥·¢£¬¹Ø±ÕÍâ²¿´¥·¢
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             		//ADCÊı¾İÓÒ¶ÔÆë
    ADC_InitStructure.ADC_NbrOfChannel = 4;                            		//4¸öÍ¨µÀ
    ADC_Init(ADC1, &ADC_InitStructure);

    /******************************************************************
       ÉèÖÃÖ¸¶¨ADCµÄ¹æÔò×éÍ¨µÀ£¬ÉèÖÃËüÃÇµÄ×ª»¯Ë³ĞòºÍ²ÉÑùÊ±¼ä
       Ê¹ÓÃADC1,Ä£ÄâÍ¨µÀx£¬²ÉÑùĞòÁĞºÅÎª  £¬²ÉÑùÊ±¼ä 55.5  ÖÜÆÚ
    *******************************************************************/
    ADC_RegularChannelConfig(ADC1,ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);

// 	¿ªÆôDMA,Ğèµ¥¶ÀÅäÖÃ
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    /* ÖØÖÃADC1µÄĞ£×¼¼Ä´æÆ÷ */
    ADC_ResetCalibration(ADC1);
    /* ¼ì²éADC1ÖØÖÃĞ£×¼¼Ä´æÆ÷µÄ×´Ì¬£¬Ö±µ½Ğ£×¼¼Ä´æÆ÷ÖØÉèÍê³É */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ¿ªÊ¼ADC1Ğ£×¼ */
    ADC_StartCalibration(ADC1);
    /*¼ì²éADC1Ğ£×¼µÄ½áÊø±êÖ¾£¬Ö±µ½Ğ£×¼Íê³É */
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/************************************************
º¯ÊıÃû³Æ £º DMA_Configuration
¹¦    ÄÜ £º DMA ³õÊ¼»¯£¬ÎÒ°Ñboard.cÀïÃæÄÇ¸ö¹Ø±ÕµôÁË
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º 1000±¶µçÑ¹Öµ
×÷    Õß £º strongerHuang
*************************************************/
/* */
void DMA_Configuration(void)//DMAÖĞ¶ÏÎ´Ìí¼Ó
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    DMA_DeInit(DMA1_Channel1); //DMA Í¨µÀ1¼Ä´æÆ÷ÉèÎªÈ±Ê¡//adc1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA ÍâÉèADC»ùµØÖ·
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMAÄÚ´æ»ùµØÖ·
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //Êı¾İ´«ÊäÄ¿µÄµØ
    DMA_InitStructure.DMA_BufferSize = 10*4; //DMA»º´æ´óĞ¡ ²É¼¯´ÎÊı*Í¨µÀÊı
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //ÍâÉèµØÖ·¼Ä´æÆ÷²»±ä
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //ÄÚ´æµØÖ·¼Ä´æÆ÷µİÔö
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //Êı¾İ¿í¶È16Î»
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //Ñ­»·Ä£Ê½
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA?? x??????
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAµ½ÄÚ´æ´«Êä
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); //³õÊ¼»¯

    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC|DMA_IT_HT,ENABLE); //´«ÊäÍê³ÉºÍ´«Êä¹ı°ë
    DMA_ClearFlag(DMA1_FLAG_GL1);	// Çå³ıDMAËùÓĞ±êÖ¾
    DMA_Cmd(DMA1_Channel1,ENABLE);	//Ê¹ÄÜÍ¨µÀ6

//	DMA_DeInit(DMA1_Channel6);//DMA1Í¨µÀ5ÅäÖÃ
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);//Ö¸ÏòUSART2//ÍâÉèµØÖ·
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA1_CH6_BUF;	//ÄÚ´æµØÖ·
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//dma´«Êä·½Ïòµ¥Ïò
//	DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;//ÔÚboard.cÖĞ¶¨Òå//ÉèÖÃDMAÔÚ´«ÊäÊ±»º³åÇøµÄ³¤¶È
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//ÉèÖÃDMAµÄÍâÉèµİÔöÄ£Ê½£¬Ò»¸öÍâÉè
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//ÉèÖÃDMAµÄÄÚ´æµİÔöÄ£Ê½
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//ÍâÉèÊı¾İ×Ö³¤
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//ÄÚ´æÊı¾İ×Ö³¤
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//ÉèÖÃDMAµÄ´«ÊäÄ£Ê½
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//VeryHigh,High,Medium,Low.	//ÉèÖÃDMAµÄÓÅÏÈ¼¶±ğ
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//¹Ø±ÕÄÚ´æµ½ÄÚ´æµÄDMAÄ£Ê½
//	DMA_Init(DMA1_Channel6,&DMA_InitStructure);
//
//	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC|DMA_IT_HT,ENABLE); //´«ÊäÍê³ÉºÍ´«Êä¹ı°ë
//	DMA_ClearFlag(DMA1_FLAG_GL6);	// Çå³ıDMAËùÓĞ±êÖ¾
//	DMA_Cmd(DMA1_Channel6,ENABLE);	//Ê¹ÄÜÍ¨µÀ6




}
/************************************************
º¯ÊıÃû³Æ £º ADC_Initializes
¹¦    ÄÜ £º ADC³õÊ¼»¯
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º ÎŞ
×÷    Õß £º strongerHuang
*************************************************/
void ADC_Initializes(void)
{
    DMA_Configuration();
    ADC_GPIO_Configuration();
    ADC_Configuration();
}

u16 GetVolt(u16 advalue)
{
    return (u16)(advalue * 330 / 4096); //Êı¾İ×ª»¡
}

/************************************************
º¯ÊıÃû³Æ £º filter
¹¦    ÄÜ £º ÂË²¨£¬Ê®´ÎÈ¡¾ùÖµ
²Î    Êı £º ÎŞ
·µ »Ø Öµ £º
×÷    Õß £º
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






