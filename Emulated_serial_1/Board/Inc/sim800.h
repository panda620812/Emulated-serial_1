#ifndef __SIM800_H__
#define __SIM800_H__	


#include "stm32f10x.h"
#include "board.h"
#include "cmsis_os2.h"
#include "stdbool.h"


enum GSM_STATUS_TYPE
{
	GSM_STATUS_PWOFF = 0,
	GSM_STATUS_STDBY = 1,
	GSM_STATUS_IPSTART = 2,
	GSM_STATUS_IPCONFIG = 3,
	GSM_STATUS_GPRSACT = 4,
	GSM_STATUS_IPSTATUS = 5,
	GSM_STATUS_PROCESS = 6,
};

struct LoopQue
{
	unsigned char buf[256];
	unsigned char buf_wp;
	unsigned char buf_rp;
};

#define HANDLER_MAX		10

struct HandlerRegister
{
	int priority;
	char * keyword;
	void (*CallBack)(struct LoopQue * lq, struct HandlerRegister * inst);
};

// 用于维护sim800的多连接状态
struct Connect
{
	uint16_t Brarer;
	uint16_t Protocol;
	uint8_t  RemoteIP[4];		/* IP address of remote machine */
	uint16_t RemotePort;		/* Port on remote machine */
	uint16_t State;
};

#define SIM800C_MAX_CONNECTS	6

void SMS_Handler(struct LoopQue* lq, struct HandlerRegister * inst);
void RING_Handler(struct LoopQue* lq, struct HandlerRegister * inst);
void RECEIVE_Handler(struct LoopQue* lq, struct HandlerRegister * inst);
char*  LoopQue_Cpy(char* dstr,struct LoopQue* lq);
extern const osThreadAttr_t sim800_attr;

extern int sim800_send_cmd(char *cmd, char *ack, s32 waittime);
extern int sim800_rec_parameter(char *cmd, char *ack, char *recbuf, int waittime);
extern void thread_sim800_rec (void *argument);





#endif
