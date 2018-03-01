#include <stm32f10x.h>
#include <ausart.h>
uint32_t delayTime = _1200BuadRate;
uint32_t  sometime = 0;

uint8_t recvStat = COM_STOP_BIT;			//定义状态机
uint8_t recvData;
//uint8_t val = 'A';
uint8_t testA = 'A';



uint8_t 	EmulateSerialBuf[GPS_RX_LEN]; //
uint16_t 	EmulateSerialLen 	= 0;
uint16_t 	EmulateSerialRx 	= 0; //接收自增
uint16_t 	RemainDataNum 		= 0;

void Delay(uint32_t t)
{
	while(t--);
}

//中断屏蔽设置
//PA 5 ---> Line5
//en:1，开启;0，关闭;  
void EmulatedInt(unsigned char en)
{
//    EXTI->PR=1<<ES_EXTI_LINE_NUM;  //清除LINE5上的中断标志位        //====> EXTI_ClearITPendingBit(EmulateSerialEXTILine);	        //清除EXTI_Line1中断挂起标志位
//    if(en)EXTI->IMR|=1<<ES_EXTI_LINE_NUM;//不屏蔽line5上的中断
//    else EXTI->IMR&=~(1<<ES_EXTI_LINE_NUM);//屏蔽line5上的中断   
}

// Timer 优先级要相对较高
void EmulatedSerialNVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);//抢占8 子2
 
/* Enable the I/O PA5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ES_EXTI_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

/* Enable the Timer Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	VirtualCOM_Config(_9600BuadRate);
}

void TimeTest()//定时器发送
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检测是否发生溢出更新事件
    {
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update); //清除中断标志
        recvStat++;				   //改变状态机
        if(recvStat == 10) 		   //收到停止位  /定时器发送测试
        {
            recvStat = 0;   //定时器发送测试
            //testA = 'A';
            COM_DATA_HIGH();

            TIM_Cmd(TIM2, DISABLE);		   //关闭定时器
            return;                            //并返回
        }

        if(recvStat == 1)
            COM_DATA_LOW();			   //???
        else
        {
            if(testA & 0x01)
            {
                testA = (testA >> 1);
                COM_DATA_HIGH();
            }
            else
            {
                testA = (testA >> 1);
                COM_DATA_LOW();
            }
        }
    }
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检测是否发生溢出更新事件
    {
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update); //清除中断标志
        recvStat++;				   													//改变状态机
        if(recvStat == COM_STOP_BIT) 		   						//收到停止位
        {
            //COM_DATA_HIGH();
            EmulateSerialBuf[EmulateSerialRx] =  recvData;
            RemainDataNum = GPS_RX_LEN - EmulateSerialRx;

            EmulateSerialRx++;
            if(EmulateSerialRx 		>= GPS_RX_LEN)
                EmulateSerialRx = 0;
            
						EmulatedInt(1);//打开外部Rx中断			
						
            TIM_Cmd(TIM2, DISABLE);	//关闭定时器
            return;               //并返回
        }
        if(COM_RX_STAT)         //'1'
        {
            recvData |= (1 << (recvStat - 1));
        }
        else				          //'0'
        {
            recvData &= ~(1 <<(recvStat - 1));
        }
    }
}
void EXTI15_10_IRQHandler(void) //EXTI15_10_IRQn
{
    if(EXTI_GetITStatus(EmulateSerialEXTILine)!=RESET)
    {
        if(!COM_RX_STAT)   //检测引脚高低电平，如果是低电平，则说明检测到下升沿
        {
					  EmulatedInt(0);//关闭中断					
					
            if(recvStat == COM_STOP_BIT)		//状态为停止位
            {
                recvStat = COM_START_BIT;	//接收到开始位
								Delay(100);			//延时一定时间
                TIM_Cmd(TIM2, ENABLE);		//打开定时器，接收数据
            }
        }
        EXTI_ClearITPendingBit(EmulateSerialEXTILine);	        //清除EXTI_Line1中断挂起标志位
    }
}

void TIM2_Configuration(unsigned short period)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

//  NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);			//抢占8 子2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能TIM2的时钟
    TIM_DeInit(TIM2);	                        					//复位TIM2定时器
    TIM_InternalClockConfig(TIM2);											//采用内部时钟给TIM2提供时钟源

    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;   		//预分频系数为72，这样计数器时钟为72MHz/72 = 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    		//设置时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置计数器模式为向上计数模式
    TIM_TimeBaseStructure.TIM_Period = period - 1;  		//设置计数溢出大小，每计period个数就产生一个更新事件
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);			//将配置应用到TIM2中

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);								//清除溢出中断标志
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);        		//开启TIM2的中断
    TIM_Cmd(TIM2,DISABLE);			        								//关闭定时器TIM2

//    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  	//通道设置为TIM2中断
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//响应式中断优先级0  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	  	//打开中断
//    NVIC_Init(&NVIC_InitStructure);
}

void VirtualCOM_RX_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    EXTI_InitTypeDef EXTI_InitStruct;

//    NVIC_InitTypeDef NVIC_InitStructure;

    /* PA5最为数据输入，模拟RX */
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);

    EXTI_InitStruct.EXTI_Line=EmulateSerialEXTILine;
    EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿都中断
    EXTI_InitStruct.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStruct);

//    NVIC_InitStructure.NVIC_IRQChannel=ES_EXTI_IRQN;  //外部中断，边沿触发
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
}
void VirtualCOM_TX_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* PA4???????,??TX */
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_TX_PORT, &GPIO_InitStructure);
    GPIO_SetBits(COM_TX_PORT, COM_TX_PIN);
}
void BSP_Init(void)
{
    static volatile ErrorStatus HSEStartUpStatus = SUCCESS;

    RCC_DeInit();     //????SYSCLK, HCLK, PCLK2, PCLK1, ????????
    RCC_HSEConfig(RCC_HSE_ON);  //????????
    HSEStartUpStatus = RCC_WaitForHSEStartUp();//????????

    if(HSEStartUpStatus == SUCCESS)
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//??flash??????
        FLASH_SetLatency(FLASH_Latency_2);   //?Flash??????,2????????
        RCC_HCLKConfig(RCC_SYSCLK_Div1);     //HCLK = SYSCLK ????????=????
        RCC_PCLK2Config(RCC_HCLK_Div1);      //PCLK2 = HCLK ??????2??=??????
        RCC_PCLK1Config(RCC_HCLK_Div2);      //PCLK1 = HCLK/2 ??????1???=????????
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLLCLK = 8MHz * 9 = 72 MHz ????????8Mhz??9???72Mhz
        RCC_PLLCmd(ENABLE);		     //??PLL???
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {} //?????????
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//?????????????
        while(RCC_GetSYSCLKSource() != 0x08) {}   //??????
    }												//??GPIO???????
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);
}


void VirtualCOM_Config(unsigned short baudRate)
{
    uint32_t period;
    VirtualCOM_TX_GPIOConfig();

    VirtualCOM_RX_GPIOConfig();
    if(baudRate == _300BuadRate)		//???300
        period = _300BuadRate + 250;
    else if (baudRate == _600BuadRate)	//???600
        period =  _600BuadRate + 50;
    else if (baudRate == _1200BuadRate)	//???1200
        period =  _1200BuadRate + 50;
    else if (baudRate == _4800BuadRate)	//???1200
        period =  _4800BuadRate + 10 ;	//
    else if (baudRate == _9600BuadRate)	//???1200
        period =  _9600BuadRate + 7 ;//103    + 1   最少+ 1，上限是13 ，看图是1.5
    TIM2_Configuration(period);		//????????????????
    delayTime = baudRate;			//??IO???????
}

void Delay_us(uint32_t nus)
{
    SysTick->LOAD=nus*9;            //????
    SysTick->CTRL|=0x01;            //????
    while(!(SysTick->CTRL&(1<<16)));//??????
    SysTick->CTRL=0X00000000;       //?????
    SysTick->VAL=0X00000000;        //?????
}

void VirtualCOM_ByteSend(uint8_t val)
{
    uint8_t i = 0;

    COM_DATA_LOW();			   //???
    Delay_us(delayTime);
    for(i = 0; i < 8; i++)	           //8????
    {
        if(val & 0x01)
            COM_DATA_HIGH();
        else
            COM_DATA_LOW();
        Delay_us(delayTime);
        val >>= 1;
    }
    COM_DATA_HIGH();		   //???
    Delay_us(delayTime);
}

void VirtualCOM_StringSend(uint8_t *str)
{
    while(*str != 0)
    {
        VirtualCOM_ByteSend(*str);
        str++;
    }
}
