/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: ʹ�ñ�׼�ⷽ������LED������
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ausart.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
void Delay(uint32_t t)
{
	while(t--);
}

/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */

uint8_t test[10] =  "testteste";
extern uint8_t recvData;
int main(void)
{  	

 	BSP_Init();

		VirtualCOM_Config(_4800BuadRate);             //??IO?????????600

	//	VirtualCOM_StringSend("HelloWorld!\r\n");   //??�HelloWorld!�???
		while(1)
		{
			//VirtualCOM_StringSend("HelloWorld!\r\n"); 
			//TIM_Cmd(TIM2, ENABLE);		//��ʱ�����Ͳ���
//      VirtualCOM_ByteSend('A');
			VirtualCOM_ByteSend(recvData);
			Delay(5000000);

		}
}

/**
  * ��������: �򵥴ֱ�����ʱ����
  * �������: time����ʱʱ������
  * �� �� ֵ: ��
  * ˵    ����ʹ��������ѭ����ʽ����ʱ������û�취��ȷ������ʱʱ�䳤�̣�
  *            ֻ�Ǵ�ŵ���ʱʱ�䣬���Ի����϶���ͨ�������޸��������
  *            ��СȻ�����ص��������Ͽ�ʵ��Ч����
  */
//static void Delay(uint32_t time)
//{
//  uint32_t i,j;

//  for(i=0;i<time;++i)
//  {
//    for(j=0;j<10000;++j)
//    {       
//      //��ѭ����ʲô������        
//    }
//  }
//}

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
