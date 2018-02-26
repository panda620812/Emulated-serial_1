# include "LCD.h"
/************************************************
函数名称 ： CRC-16/IBM 校验 	x16+x15+x2+1
功    能 :	232串口
参    数 ： 
返 回 值 ： 
作    者 ： 
*************************************************/
struct DataLCDTxType1 LCDTest;
//CRC-16/IBM         
#define PLOY 0xa001 //0x8005 反转//crc16(a,sizeof(a)/sizeof(int)) a是数组
unsigned short crc16(int *data,int size) {
	unsigned short crc = 0x0;
	int data_t ;
	int i = 0;
	int j = 0;
	if(data ==NULL){
		return 0;
	}
	for(j=0;j<size;j++){
		data_t = *data++;
		crc = (data_t ^ (crc));
		for(i=0;i<8;i++){
			if((crc&0x1)==1){
			crc = (crc>>1) ^ PLOY;
			}
			else{
				crc >>= 1;
			}	
		}
	}
	return crc;
}	
/************************************************
函数名称 ： 初始化发送帧1和帧2
功    能 :	232串口，数据获取可以根据软件动态获取
参    数 ： 
返 回 值 ： 
作    者 ： 
*************************************************/
//串口1发送
void LCDAsk(void)          //发送结束标志
{
	uint16_t i;
	uint16_t eof[4]={0xcc,0x33,0xc3,0x3c};

	 
	for(i=0;i<4;i++)
	{
		USART_SendData(USART1,eof[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);  //当状态regist，USART_SR的	TC为set表示发送完成
	}
}

/*
       //串口1发送1字节数据
void send_byte(u8 data)
{
	 USART_SendData(USART1,data);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
}

        //串口1发送s个字符
void send_str(u8 *str,u8 s)
{
	u8 i;
	for(i=0;i<s;i++)
	{
		send_byte(*str);
		str++;
	}
}




void wenbenchange1(void)
{
	u8 i,j;

 	send_str(clr,2);    //清屏
   en();
	delay_ms(100);
	for(j=0;j<8;j++)
	{
		color[2]=0xf8;      //设置前景色  红
		color[5]=0x1f;      //设置背景色   蓝
		send_str(color,6);   //设置前景色  红   背景色   蓝
		en();
		delay_ms(10);
		wenben1[6]=0x31+j;     //显示数字  1+j
		wenben1[5]=0x0a+28*j;   //y坐标10+28*j，x坐标不变  使显示坐标下移28个点；
		send_str(wenben1,20);
		en();
		delay_ms(100);
		color[5]=0x00;    //设置背景色    黑
		send_str(color,6);   //设置前景色   红   背景色    黑
		en();
		
		delay_ms(10);
		
		for(i=0;i<8;i++)
		{
			
			wenben1[6]=0x31+i;     //显示数字    1+i；
			wenben1[5]=0x0a+28*i;   //y坐标下移  28+i；
			send_str(wenben1,20);    //显示
			en();
			
			delay_ms(50);
		}
		
	}
}


void wenbenchange2(void)
{
	u8 i,j;
	send_str(clr,2);
	en();
	delay_ms(100);
	for(j=0;j<8;j++)
	{
		color[2]=0xf8;
		color[5]=0x1f;
		send_str(color,6);
		en();
		delay_ms(10);
		if(j==0){wenben2[6]=0x20;wenben2[7]=0x39;}
		else{wenben2[6]=0x31;wenben2[7]=0x30+j-1;}
		wenben2[5]=0x0a+28*j;
		send_str(wenben2,20);
		en();
		delay_ms(100);
		color[5]=0x00;
		send_str(color,6);
		en();
		delay_ms(10);
		for(i=0;i<8;i++)
		{
			if(i==0){wenben2[6]=0x20;wenben2[7]=0x39;}
			else {wenben2[6]=0x31;wenben2[7]=0x30+i-1;}
			wenben2[5]=0x0a+28*i;
			send_str(wenben2,20);
			en();
			delay_ms(50);
		}
	}
}

//lcd握手指令
void acklcd(void)
{
	USART_RX_STA=0;
	while(USART_RX_STA==0)      //等待LCD 就绪
	{
		pic[1]=0x00;
		send_str(pic,2);
		en();               //发送握手指令
		delay_ms(10);
	}
	USART_RX_STA=0;        //复位有效数据状态位
}
*/		
/*
struct Testt
    {
        int *a;
        int size_a;
        char *b;
        int size_b;
    };
 
    Testt t;
    t.size_a = 10;
    t.size_b = 20;
    t.a = (int *)malloc(sizeof(int)*t.size_a);
    t.a[1]=0;
    t.a[9]=20;
    CString str;
    str.Format("%d",t.a[9]);
    AfxMessageBox(str);
    free(t.a);
*/
