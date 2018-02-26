#ifndef	_USART_H_
#define	_USART_H_

//extern void USART_PRINT(USART_TypeDef* USARTx, char *Data);
//extern void USART_SEND(USART_TypeDef* USARTx, s8 *Data, s32 len);
//sn65hvd3082e 0 ���� 1 ����
//485���� ����λ
#define RX_MODE_USART2 	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
#define RX_MODE_UART5 	GPIO_ResetBits(GPIOB,GPIO_Pin_3);

#define TX_MODE_USART2 	GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define TX_MODE_UART5 	GPIO_SetBits(GPIOB,GPIO_Pin_3);


//��������
void USART_SEND(USART_TypeDef* USARTx, s8 *Data, s32 len);
void USART_Configuration(void);
void USARTx_SendNByte(USART_TypeDef* USARTx	,uint8_t *pData, uint16_t Length);
void USARTx_SendByte(USART_TypeDef* USARTx	,	uint8_t Data);
void USART_PRINT(USART_TypeDef* USARTx, char *Data)	;
//�жϺ���
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
#pragma pack(2)//�޸Ķ��뷽ʽ������Ϊ�����ֽ���

struct DataTxType{				//���ݴ������֡********
    uint8_t	 	Startflag;      //1  
	uint8_t 	SendChannel;    //1     	
    uint8_t		FunctionCode;   //1       	
    uint8_t 	DataLengh;    	//1	4 
// 	uint8_t  	Data;            
	uint16_t	PM2_5;			//2 6
	uint32_t	PM10;			//4 10
//	uint16_t	Temp;			//2 
	int16_t 	Temp;			//2�ֽڣ�������
	uint16_t	Humidity;//ʪ��	//2
	uint16_t	kPa ;			//2 
	uint16_t	WindSpeed;		//2 
	uint16_t	WindDirection;	//2 
	uint16_t	Noise;			//2	22 
//	uint8_t  	Crc16L;         
//  uint8_t  	Crc16H;     	
	uint16_t	Crc16;			//2 23 �˴�ռ�ռ�Ϊ32
};
#pragma pack()//����ΪĬ�϶��뷽ʽ
#pragma pack(2)
struct DataPMType{				//PM���ݴ������֡��ע��ҲҪ���ö����ʽ
    uint8_t	 	Startflag;      //1  
	uint8_t 	SendChannel;    //1     	
    uint8_t		FunctionCode;   //1       	
    uint8_t 	DataLengh;    	//1	4 
   	uint16_t	PM2_5;			//2 6
	uint32_t	PM10;			//4 10
	uint16_t	Temp;			//2 
	uint16_t	Humidity;//ʪ��	//2
	uint16_t	kPa ;			//2 
	uint16_t	WindSpeed;		//2 
	uint16_t	WindDirection;	//2 
	uint16_t	Noise1;			//2	22 
	uint16_t	Crc16;			//2 23 �˴�ռ�ռ�Ϊ32
};
#pragma pack()//����ΪĬ�϶��뷽ʽ

#pragma pack(2)
struct DataPMNoiseType{			//PM10+NOise2���Ƿ�����ȷ��	�����ϴ�
								//����	��Ӧֵ
    uint8_t	 	Startflag;      //1  	AA
	uint8_t 	SendChannel;    //1     02	
    uint8_t		FunctionCode;   //1     02  	
    uint8_t 	DataLengh;    	//1	4 	06
 	uint32_t	PM10;			//4 8
	uint16_t	Noise2;			//2	10 
	uint16_t	Crc16;			//2 12 �˴�ռ�ռ�Ϊ����
};
#pragma pack()//����ΪĬ�϶��뷽ʽ
//RFID ����������������
#pragma pack(1)
struct DataRFIDRxType{			//RFID��Ϣ ���ղ���
								//����	��Ӧֵ ����ֽڿ��� 10�ֽ�ASCII�뷢�ͣ�˳����������ܺͷ����Ƕ�Ӧ�ģ������ͺ���
    uint16_t	Startflag; 		//2  	0x30,0x32
	uint16_t 	ID;    			//2     02			ASCII
    uint8_t		Data[10];    	//10 	0A
	uint16_t	Crc16;			//2 	
	uint16_t	ETX;			//2		0x0D 0x0A
};
#pragma pack()//����ΪĬ�϶��뷽ʽ
#pragma pack(1)
struct DataRFIDTxType{			//RFID��Ϣ���Ͳ���********
								//����	��Ӧֵ��������Ÿ��ֽ���ǰ
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
#pragma pack()//����ΪĬ�϶��뷽ʽ
//PM �������ղ���
#pragma pack(2)
struct DataPM25PM10RxType{				//PM��Ϣ ���ղ��֣�Ĭ�������ϴ�
    uint16_t	Startflag; 		//1  	AA
	uint16_t 	InsNum;    		//1     C0			ָ���
    uint8_t		PM25L;    		//1	 	
	uint8_t		PM25H;    		//1	 	
	uint8_t		PM10L;    		//1	 	
	uint8_t		PM10H;  		//1
	uint8_t		ID1;			//1 
	uint8_t		ID2;			//1 	
	uint8_t		ETX;			//2		AB
};
#pragma pack()//����ΪĬ�϶��뷽ʽ

#pragma pack(2)
struct DataPMNoiseTxType{				//PM10+Noise2�����ϴ�����
    uint8_t	 	Startflag;      //1  	AA
	uint8_t 	SendChannel;    //1     02	
    uint8_t		FunctionCode;   //1     02 	
    uint8_t 	DataLengh;    	//1	4 	06
	uint32_t	PM10;  			//4
	uint16_t	Noise2;			//2 
	uint16_t	Crc;			//2
};
#pragma pack()//����ΪĬ�϶��뷽ʽ

#pragma pack(2)
struct DataTempPaRxType{		//�����߷������¶�ѹ���������֣�485 ��ѯʽ
    uint16_t	Startflag; 		//2  	AA BB
	uint8_t 	Adress;    		//1     E2		ָ���
    uint8_t		DateGetNum;    	//1	 	F1
	uint8_t		StaticFlag;		//1		0A
	uint8_t		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second ;
	float		DateGet;		//				��������
	float		Temp;
	float		Pa;				//				ѹ��
	float		Flow;			//				����
	uint8_t		Flag;			//				��־�� 00ֽ������ 01 ��ֽ
	uint16_t	Crc;			//2 			Ĭ��FFFF
	uint16_t	Endcode;		//				CC DD
};
#pragma pack()//����ΪĬ�϶��뷽ʽ

#pragma pack(1)
struct DataXhpRxType{		//�»��� 485 ��ѯʽ�������ã������ ���ֽ���ǰ�һ�����ô????
/*  uint8_t 	Adress;    		//1     E2		ָ���
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
#pragma pack()//����ΪĬ�϶��뷽ʽ
#pragma pack(1)
struct DataSonicRxType{		//������Һλ��  485 ��ѯʽ�����ΪAscii��
  	uint8_t 	ProtocolHeader[3];    	//   	"*XD"		
    uint8_t		ProtocolVersion;    	//	 	"4"
	uint16_t	SoftVersion;			//		"14"
	uint8_t		HardVersion;			//		"G"
	uint16_t	Keep1;					//		"01"		
	uint8_t		Data[5];				
	uint16_t	WaveNum;				//�������ز�����
	uint8_t		Softstate;				//		��0��
	uint8_t		Hardstate;				//		��0��		
	uint16_t	Keep2;					//		"65"
	uint16_t	Keep3;					//		"06"
	uint8_t		Data2[5];				//����ֵ
	uint8_t		Null;					//null	
	uint16_t	Adress;					//	
	uint8_t		Movestatus;				//	1 ��ֹ��0 �˶�
	uint8_t		Range;					//
	uint8_t		Sumcheck;				// ��* ��
	uint8_t		ProtocolEnd;			
	
};
#pragma pack()//����ΪĬ�϶��뷽ʽ
#pragma pack(1)
struct DataAWA5636RxType{				// NOISE  RS232 ��ѯʽ�����ΪAscii��
  uint8_t 	ProtocolHeader[4];    	//   	��AWAM�� 2012-01-25 10:15:45 56.3dBA,5.5V У���
	uint8_t		Date [39];					//��ȷ��
	uint16_t	Sumcheck;	
	
	
};
#pragma pack()//����ΪĬ�϶��뷽ʽ
#endif
