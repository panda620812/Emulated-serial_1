#define COM_TX_PORT	GPIOA
#define COM_RX_PORT	GPIOA
#define COM_TX_PIN	GPIO_Pin_4
#define COM_RX_PIN	GPIO_Pin_5

#define COM_DATA_HIGH()	GPIO_SetBits(COM_TX_PORT, COM_TX_PIN)    //???
#define COM_DATA_LOW()	GPIO_ResetBits(COM_TX_PORT, COM_TX_PIN)  //???

#define COM_RX_STAT	GPIO_ReadInputDataBit(COM_RX_PORT, COM_RX_PIN)

#define u16 unsigned short
#define u8 unsigned char
#define u32 unsigned int

#define _300BuadRate	3150
#define _600BuadRate	1700
#define _1200BuadRate	800
#define _4800BuadRate	208
#define _9600BuadRate	103

#define GPS_RX_LEN	128

enum
{
    COM_START_BIT,		         //???
    COM_D0_BIT,			 //bit0
    COM_D1_BIT,			 //bit1
    COM_D2_BIT,			 //bit2
    COM_D3_BIT,			 //bit3
    COM_D4_BIT,			 //bit4
    COM_D5_BIT,			 //bit5GPS_RX_LEN - EmulateSerialRx
    COM_D6_BIT,			 //bit6
    COM_D7_BIT,			 //bit7
    COM_STOP_BIT,	         //bit8
};
void EmulatedSerialNVIC_Configuration(void);
void VirtualCOM_ByteSend(uint8_t val);
void VirtualCOM_TX_GPIOConfig(void);
void BSP_Init(void);
void VirtualCOM_Config(u16 baudRate);

void Delay_us(uint32_t nus);
void VirtualCOM_StringSend(uint8_t *str);

void VirtualCOM_RX_GPIOConfig(void);
void TIM2_Configuration(u16 period);
void EXTI9_5_IRQHandler(void);
void TIM2_IRQHandler(void);

void thread_gps (void );

