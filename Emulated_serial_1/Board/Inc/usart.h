#ifndef	_USART_H_
#define	_USART_H_

//extern void USART_PRINT(USART_TypeDef* USARTx, char *Data);
//extern void USART_SEND(USART_TypeDef* USARTx, s8 *Data, s32 len);
//sn65hvd3082e 0 接受 1 发送
//485串口 控制位
#define RX_MODE_USART2 	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
#define RX_MODE_UART5 	GPIO_ResetBits(GPIOB,GPIO_Pin_3);

#define TX_MODE_USART2 	GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define TX_MODE_UART5 	GPIO_SetBits(GPIOB,GPIO_Pin_3);


//辅助函数
void USART_SEND(USART_TypeDef* USARTx, s8 *Data, s32 len);
void USART_Configuration(void);
void USARTx_SendNByte(USART_TypeDef* USARTx	,uint8_t *pData, uint16_t Length);
void USARTx_SendByte(USART_TypeDef* USARTx	,	uint8_t Data);
void USART_PRINT(USART_TypeDef* USARTx, char *Data)	;
//中断函数
void USART2_IRQHandler(void);
void USART1_IRQHandler(void);
void USART3_IRQHandler (void);
void UART4_IRQHandler(void);
void UART5_IRQHandler (void);
void DMA1_Channel6_IRQHandler(void);

// float temp1 = 0;
// float temp2 = 0;
// float temp3 = 0;
// float temp4 = 0;
// float temp5 = 0;
#pragma pack(2)//修改对齐方式，参数为对齐字节数

struct DataTxType{				//数据打包发送帧********
    uint8_t	 	Startflag;      //1  
	uint8_t 	SendChannel;    //1     	
    uint8_t		FunctionCode;   //1       	
    uint8_t 	DataLengh;    	//1	4 
// 	uint8_t  	Data;            
	uint16_t	PM2_5;			//2 6
	uint32_t	PM10;			//4 10
//	uint16_t	Temp;			//2 
	int16_t 	Temp;			//2字节，测试用
	uint16_t	Humidity;//湿度	//2
	uint16_t	kPa ;			//2 
	uint16_t	WindSpeed;		//2 
	uint16_t	WindDirection;	//2 
	uint16_t	Noise;			//2	22 
//	uint8_t  	Crc16L;         
//  uint8_t  	Crc16H;     	
	uint16_t	Crc16;			//2 23 此处占空间为32
};
#pragma pack()//重置为默认对齐方式
#pragma pack(2)
struct DataPMType{				//PM数据打包接收帧，注意也要设置对齐格式
    uint8_t	 	Startflag;      //1  
	uint8_t 	SendChannel;    //1     	
    uint8_t		FunctionCode;   //1       	
    uint8_t 	DataLengh;    	//1	4 
   	uint16_t	PM2_5;			//2 6
	uint32_t	PM10;			//4 10
	uint16_t	Temp;			//2 
	uint16_t	Humidity;//湿度	//2
	uint16_t	kPa ;			//2 
	uint16_t	WindSpeed;		//2 
	uint16_t	WindDirection;	//2 
	uint16_t	Noise1;			//2	22 
	uint16_t	Crc16;			//2 23 此处占空间为32
};
#pragma pack()//重置为默认对齐方式

#pragma pack(2)
struct DataPMNoiseType{			//PM10+NOise2，是否对齐待确认	主动上传
								//长度	对应值
    uint8_t	 	Startflag;      //1  	AA
	uint8_t 	SendChannel;    //1     02	
    uint8_t		FunctionCode;   //1     02  	
    uint8_t 	DataLengh;    	//1	4 	06
 	uint32_t	PM10;			//4 8
	uint16_t	Noise2;			//2	10 
	uint16_t	Crc16;			//2 12 此处占空间为？？
};
#pragma pack()//重置为默认对齐方式
//RFID 主动接收主动发送
#pragma pack(1)
struct DataRFIDRxType{			//RFID信息 接收部分
								//长度	对应值 五个字节卡号 10字节ASCII码发送，顺序不清楚，可能和发送是对应的，那样就好了
    uint16_t	Startflag; 		//2  	0x30,0x32
	uint16_t 	ID;    			//2     02			ASCII
    uint8_t		Data[10];    	//10 	0A
	uint16_t	Crc16;			//2 	
	uint16_t	ETX;			//2		0x0D 0x0A
};
#pragma pack()//重置为默认对齐方式
#pragma pack(1)
struct DataRFIDTxType{			//RFID信息发送部分********
								//长度	对应值，车辆编号高字节在前
    uint8_t	 	Startflag;      //1  	AA
	uint8_t 	SendChannel;    //1     02	
    uint8_t		FunctionCode;   //1     04 	
    uint8_t 	DataLengh;    	//1	4 	0a
	uint8_t		Car1H;
	uint8_t		Car1L;
	uint8_t		Car2H;
	uint8_t		Car2L;	
	uint8_t		Car3H;
	uint8_t		Car3L;
	uint8_t		Car4H;
	uint8_t		Car4L;	
	uint8_t		Car5H;
	uint8_t		Car5L;	
 	uint16_t	Crc16;			//2 	L H 

};
#pragma pack()//重置为默认对齐方式
//PM 主动接收部分
#pragma pack(2)
struct DataPM25PM10RxType{				//PM信息 接收部分，默认主动上传
    uint16_t	Startflag; 		//1  	AA
	uint16_t 	InsNum;    		//1     C0			指令号
    uint8_t		PM25L;    		//1	 	
	uint8_t		PM25H;    		//1	 	
	uint8_t		PM10L;    		//1	 	
	uint8_t		PM10H;  		//1
	uint8_t		ID1;			//1 
	uint8_t		ID2;			//1 	
	uint8_t		ETX;			//2		AB
};
#pragma pack()//重置为默认对齐方式

#pragma pack(2)
struct DataPMNoiseTxType{				//PM10+Noise2主动上传部分
    uint8_t	 	Startflag;      //1  	AA
	uint8_t 	SendChannel;    //1     02	
    uint8_t		FunctionCode;   //1     02 	
    uint8_t 	DataLengh;    	//1	4 	06
	uint32_t	PM10;  			//4
	uint16_t	Noise2;			//2 
	uint16_t	Crc;			//2
};
#pragma pack()//重置为默认对齐方式

#pragma pack(2)
struct DataTempPaRxType{		//β射线法接收温度压力流量部分，485 问询式
    uint16_t	Startflag; 		//2  	AA BB
	uint8_t 	Adress;    		//1     E2		指令号
    uint8_t		DateGetNum;    	//1	 	F1
	uint8_t		StaticFlag;		//1		0A
	uint8_t		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second ;
	float		DateGet;		//				测量数据
	float		Temp;
	float		Pa;				//				压力
	float		Flow;			//				流量
	uint8_t		Flag;			//				标志吗 00纸带正常 01 断纸
	uint16_t	Crc;			//2 			默认FFFF
	uint16_t	Endcode;		//				CC DD
};
#pragma pack()//重置为默认对齐方式

#pragma pack(1)
struct DataXhpRxType{		//新惠普 485 问询式，可设置，待添加 低字节在前我还处理么????
/*  uint8_t 	Adress;    		//1     E2		指令号
    uint8_t		Function;    	//1	 	F1
	uint8_t		Data1L;			//1		0A
	uint8_t		Data1H;
*/
	uint8_t		Header[4];	
	uint16_t	Data2;
	uint16_t	Data3;
//	uint8_t		Data3L;
//	uint8_t		Data3H;
	uint8_t		CheckL;			//				
	uint8_t		CheckH;			//	
};
#pragma pack()//重置为默认对齐方式
#pragma pack(1)
struct DataSonicRxType{		//超声波液位仪  485 问询式，输出为Ascii码
  	uint8_t 	ProtocolHeader[3];    	//   	"*XD"		
    uint8_t		ProtocolVersion;    	//	 	"4"
	uint16_t	SoftVersion;			//		"14"
	uint8_t		HardVersion;			//		"G"
	uint16_t	Keep1;					//		"01"		
	uint8_t		Data[5];				
	uint16_t	WaveNum;				//激励波回波个数
	uint8_t		Softstate;				//		“0”
	uint8_t		Hardstate;				//		“0”		
	uint16_t	Keep2;					//		"65"
	uint16_t	Keep3;					//		"06"
	uint8_t		Data2[5];				//集中值
	uint8_t		Null;					//null	
	uint16_t	Adress;					//	
	uint8_t		Movestatus;				//	1 静止，0 运动
	uint8_t		Range;					//
	uint8_t		Sumcheck;				// 从* 到
	uint8_t		ProtocolEnd;			
	
};
#pragma pack()//重置为默认对齐方式
#pragma pack(1)
struct DataAWA5636RxType{				// NOISE  RS232 问询式，输出为Ascii码
  uint8_t 	ProtocolHeader[4];    	//   	“AWAM” 2012-01-25 10:15:45 56.3dBA,5.5V 校验和
	uint8_t		Date [39];					//待确认
	uint16_t	Sumcheck;	
	
	
};
#pragma pack()//重置为默认对齐方式
#endif
