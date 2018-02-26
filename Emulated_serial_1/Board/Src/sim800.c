#include "sim800.h"
#include <stdio.h>
#include "string.h"

// ���λ�����
struct LoopQue SIM800_LQ;
char MsgBuf[256];
char Rx_Pack_Flag, Catch_Pack_Flag;

bool Comm_Idle;
unsigned int USART_RX_STATUS=0;
char viewtest;
enum GSM_STATUS_TYPE SIM800_Status;
int Interrnet_Connect;
char ReceiveBuf[256];
char InfoString[128];
int LocalIP[4];
int IP_STATE;


struct Connect SIM800_Connects[SIM800C_MAX_CONNECTS];
int sim800_check_connect_state(void);

struct HandlerRegister HandlerRegs[HANDLER_MAX]=
{
    {0, "\r\n+CMTI", 	SMS_Handler},
    {0, "\r\nRING\r\n", RING_Handler},
    {0, "\r\n+RECEIVE,",RECEIVE_Handler},
    {0, NULL,			NULL},
    {0, NULL,			NULL},
    {0, NULL,			NULL},
    {0, NULL,			NULL},
    {0, NULL,			NULL},
};



__NO_RETURN void thread_sim800c (void *argument)
{
    osThreadId_t tid;
    int res, delay_cnt;
    int conn_try, regist_try;
    conn_try = 0;
    regist_try = 0;
    osDelay (3000);
    // ����sim800���ݽ��ա������߳�
    tid = osThreadNew(thread_sim800_rec, NULL, &sim800_attr);
    if(tid == NULL)
    {
        DebugLog("osThreadNew: thread_sim800_rec ,ERROR");
        while(1);
    }
    Interrnet_Connect = 1;
    delay_cnt = 0;
    DebugLog("THREAD:thread_sim800c RUNNING");
    while(1)
    {


        switch(SIM800_Status)
        {
        case GSM_STATUS_PWOFF:									// �ػ�״̬
            res = 0;
            res += sim800_send_cmd("AT", "OK", 500);
            res += sim800_send_cmd("ATE0", "OK", 500);
            if(res == 0)
            {   // AT �ظ� OK
                SIM800_Status = GSM_STATUS_STDBY;
                delay_cnt = 0;
                DebugLog("�������OK");
            }
            else
            {   // ģ��û�п���������ͨ�Ŵ��ڹ���
                DebugLog("δ��⵽SIM800");
                osDelay (2000);
            }
            break;
        case GSM_STATUS_STDBY:									// ����״̬
            delay_cnt ++ ;
            if(Interrnet_Connect == 1)
            {   // ��Ҫ��ʼ���ӻ�����
                SIM800_Status = GSM_STATUS_IPSTART;
                delay_cnt = 0;
            }

            if(delay_cnt % 5000 == 0)
            {   // ��������ʱÿ5s���һ��
                res = sim800_send_cmd("AT", "OK", 500);
                if(res == 0)
                {   // AT �ظ� OK�����ִ���
                    DebugLog("�������OK");
                }
                else
                {   // AT û�лظ�
                    SIM800_Status = GSM_STATUS_PWOFF;
                    DebugLog("����");
                }
            }
            break;
        case GSM_STATUS_IPSTART:								// ����GPRS����
            res = 0;
            res += sim800_send_cmd("AT+CIPSHUT", "SHUT OK", 1000);
            res += sim800_send_cmd("AT+CGCLASS=\"B\"", "OK", 1000);
            res += sim800_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"", "OK", 1000);
            res += sim800_send_cmd("AT+CGATT=1", "OK", 500);
            res += sim800_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500);
            res += sim800_send_cmd("AT+CIPMUX=1","OK",500);
            res += sim800_send_cmd("AT+CSTT=\"CMNET\"","OK",500);
            if(res == 0)
            {
                DebugLog("IP START����");
            }
            else
            {
                SIM800_Status = GSM_STATUS_STDBY;
                DebugLog("IP START ���ò���ȷ");
                break;
            }
            res = sim800_send_cmd("AT+CGATT?", "+CGATT: 1\r\n\r\nOK", 500);
            if(res == 0)
            {
                DebugLog("����GPRSҵ��");
            }
            else
            {
                SIM800_Status = GSM_STATUS_STDBY;
                DebugLog("MT�Ѵ�GPRSҵ�����");
                break;
            }
            res = sim800_send_cmd("AT+CIICR","OK",1000);
            if(res == 0)
            {
                delay_cnt = 0;
                SIM800_Status = GSM_STATUS_GPRSACT;
                DebugLog("����GPRS");
            }
            else
            {
                SIM800_Status = GSM_STATUS_STDBY;
                DebugLog("�޷�����GPRS");
                break;
            }

            break;

        case GSM_STATUS_GPRSACT:								// �������Ѽ���
            res = sim800_rec_parameter("AT+CIFSR", NULL, ReceiveBuf, 500);
            if(res == 0)
            {
                DebugLog("��ȡ����IP��ַ");
                res = sscanf(ReceiveBuf, "\r\n%d.%d.%d.%d\r\n", &LocalIP[0], &LocalIP[1], &LocalIP[2], &LocalIP[3]);
                if(res == 4)
                {
                    SIM800_Status = GSM_STATUS_IPSTATUS;
                    DebugLog("��������IP��ַ�ɹ�");
                    DebugLog("%d.%d.%d.%d", LocalIP[0], LocalIP[1], LocalIP[2], LocalIP[3]);
                    break;
                }
                else
                {
                    SIM800_Status = GSM_STATUS_STDBY;
                    DebugLog("��������IP��ַʧ�ܣ�");
                    DebugLog("%s",ReceiveBuf);
                    break;
                }
            }
            else
            {
                SIM800_Status = GSM_STATUS_STDBY;
                DebugLog("��ȡ����IP��ַ����Ӧ��");
            }
            break;
        case GSM_STATUS_IPSTATUS:
            delay_cnt++;
            res = sim800_rec_parameter("AT+CIPSTART=0,\"TCP\",\"jndzwt.vicp.cc\",\"4000\"", "0, CONNECT OK", ReceiveBuf, 500);
            if(res == 0)
            {
                delay_cnt = 0;
                SIM800_Status = GSM_STATUS_PROCESS;
                DebugLog("��������0�ɹ�");
            }
            else
            {
                DebugLog("��������0����:%s",ReceiveBuf);
            }
            break;
        case GSM_STATUS_PROCESS:
            delay_cnt++;
            osDelay (2000);
            sim800_check_connect_state();
            osDelay (2000);
            break;
        default:
            SIM800_Status = GSM_STATUS_PWOFF;
            delay_cnt = 0;
        }
        osDelay (1);
        viewtest ++;
    }
}

char item[64];

int sim800_check_connect_state(void)
{
    int res, length, i;
    int sn, bearer, ip0,ip1,ip2,ip3, port;
    char tmpstr[8];
    char * recstr;
    char * label;
    recstr = ReceiveBuf;
    res = sim800_rec_parameter("AT+CIPSTATUS", "\r\nSTATE:", recstr, 500);
    if(res == 0)
    {
        // �����յ����ַ�����
        label = "\r\nSTATE:";
        recstr = strstr(recstr, label);
        recstr += strlen(label);
        length = strcspn(recstr, "\r\n");
        strncpy(item, recstr, length);
        if(strstr(item, "IP INITIAL"))
        {
            IP_STATE = 0;
        }
        else if(strstr(item, "IP START"))
        {
            IP_STATE = 1;
        }
        else if(strstr(item, "IP CONFIG"))
        {
            IP_STATE = 2;
        }
        else if(strstr(item, "IP GPRSACT"))
        {
            IP_STATE = 3;
        }
        else if(strstr(item, "IP STATUS"))
        {
            IP_STATE = 4;
        }
        else if(strstr(item, "IP PROCESSING"))
        {
            IP_STATE = 5;
        }
        else if(strstr(item, "PDP DEACT"))
        {
            IP_STATE = 9;
        }
        else
        {   // δ֪ IP ״̬
            IP_STATE = -1;
        }
        label = "\r\n\r\n";
        recstr = strstr(recstr, label);
        recstr += strlen(label);
        for(i= 0; i< SIM800C_MAX_CONNECTS; i++)
        {   // ����6�����ӵ�״̬
            label = ":";
            recstr = strstr(recstr, label);
            recstr += strlen(label);
            res = sscanf(recstr, " %d,%d,\"%3s\",\"%d.%d.%d.%d\",\"%d\",\"%s\"",
                         &sn, &bearer,tmpstr, &ip0, &ip1, &ip2, &ip3, &port, item);
            if(res == 9)
            {
                SIM800_Connects[i].Brarer = bearer;
                SIM800_Connects[i].RemoteIP[0] = ip0;
                SIM800_Connects[i].RemoteIP[1] = ip1;
                SIM800_Connects[i].RemoteIP[2] = ip2;
                SIM800_Connects[i].RemoteIP[3] = ip3;
                SIM800_Connects[i].RemotePort = port;
            }
            else
            {
                break;
            }

            label = "\r\n";
            recstr = strstr(recstr, label);
            recstr += strlen(label);
        }
    }
    return 0;
}


void SMS_Handler(struct LoopQue* lq, struct HandlerRegister * inst)
{
    int offset;

    offset = inst->priority;
    lq->buf_rp += offset;
    LoopQue_Cpy(MsgBuf, lq);
    DebugLog("�յ�����:%s", MsgBuf);
}

void RING_Handler(struct LoopQue* lq, struct HandlerRegister * inst)
{
    // �ܽ���������
    sim800_send_cmd("ATH", NULL, 0);
    DebugLog("���磬�Ѿܽ�");

}

void RECEIVE_Handler(struct LoopQue* lq, struct HandlerRegister * inst)
{
    int offset;

    offset = inst->priority;
    lq->buf_rp += offset;
    LoopQue_Cpy(MsgBuf, lq);
    DebugLog("���ݰ�:%s", MsgBuf);
}


// ���û��ζ���Ϊ��ʼ״̬��
// �������ζ�����δȡ��������
void LoopQue_init(struct LoopQue* lq)
{
    lq->buf_rp = lq->buf_wp;
    lq->buf[lq->buf_wp] = 0;
}

char*  LoopQue_Cpy(char* dstr,struct LoopQue* lq)
{
    *dstr = lq->buf[lq->buf_rp];

    if(lq->buf_rp == lq->buf_wp)
    {
        return dstr;
    }

    do
    {
        dstr++;
        lq->buf_rp++;
        *dstr = lq->buf[lq->buf_rp];
    }
    while(lq->buf_rp != lq->buf_wp);

    return dstr;
}

// �ӻ��ζ��л������У������ַ���,
// �����ڷ���ƫ�������������ڷ���-1
int LoopQue_Str(struct LoopQue* lq, char* sub_str)
{
    u8* t2 = NULL;
    u8  c  = NULL;
    u8  c2 = NULL;
    u8  rp = lq->buf_rp;
    int offset = 0;

    if ((c = *sub_str++) == NULL)        /*<find> an empty string*/
        return 0;

    while(1)
    {
        c2 = lq->buf[rp];
        while ((rp != lq->buf_wp) && (c2 != NULL) && (c2 != c))
        {
            rp++;
            offset++;
            c2 = lq->buf[rp];
        }

        if ((c2 == NULL) || (rp == lq->buf_wp))
        {
            return (-1);
        }
        rp++;
        t2 = (u8*)(sub_str);
        c2 = *t2;
        while ((c2  != 0) && (lq->buf[rp] == c2))
        {
            t2++;
            rp++;
            c2 = *t2;
        }

        if (c2 == NULL)
        {
            return (offset);
        }
    }
}

int sim800_send_cmd(char *cmd, char *ack, s32 waittime)
{
    int res = -1;
    int strx= -1;

    LoopQue_init(&SIM800_LQ);
    Catch_Pack_Flag = 1;
    USART_PRINT(USART2, cmd);
    USART_PRINT(USART2, "\r\n");

    if(waittime && ack)	//��Ҫ�ȴ�Ӧ��
    {
        while(waittime > 0)
        {
            if(Rx_Pack_Flag == 1)
            {
                Rx_Pack_Flag = 0;
                strx=LoopQue_Str(&SIM800_LQ,(char*)ack);
                if(strx != -1)
                {
                    res = 0;		// �յ�������Ӧ��
                    goto finally;
                }
                strx=LoopQue_Str(&SIM800_LQ,"+CME ERROR");
                if(strx != -1)
                {
                    res = 3;		// �յ�CME ERRORӦ��
                    goto finally;
                }
                strx=LoopQue_Str(&SIM800_LQ,"ERROR");
                if(strx != -1)
                {
                    res = 2;		// �յ�ERRORӦ��
                    goto finally;
                }
            }
            osDelay (10);
            waittime--;
        }
        if(waittime==0)
            res=1; 				// ���ճ�ʱ
    }
finally:
    Catch_Pack_Flag = 0;
    LoopQue_init(&SIM800_LQ);
    return res;
}


int sim800_rec_parameter(char *cmd, char *ack, char *recbuf, int waittime)
{
    int res = -1;
    int strx= -1;

    LoopQue_init(&SIM800_LQ);
    Catch_Pack_Flag = 1;

    USART_PRINT(USART2, cmd);
    USART_PRINT(USART2, "\r\n");
    // ������Ҫ��Ӧ��
    while(waittime > 0)
    {
        if(Rx_Pack_Flag == 1)
        {
            Rx_Pack_Flag = 0;
            if(ack != NULL)
            {   // �̶�����Ӧ����
                strx=LoopQue_Str(&SIM800_LQ,(char*)ack);
                if(strx != -1)
                {
                    LoopQue_Cpy(recbuf, &SIM800_LQ);
                    res = 0;		// �յ�������Ӧ��
                    goto finally;
                }
                strx=LoopQue_Str(&SIM800_LQ,"+CME ERROR");
                if(strx != -1)
                {
                    LoopQue_Cpy(recbuf, &SIM800_LQ);
                    res = 3;		// �յ�CME ERRORӦ��
                    goto finally;
                }
                strx=LoopQue_Str(&SIM800_LQ,"\r\nERROR\r\n");
                if(strx != -1)
                {
                    LoopQue_Cpy(recbuf, &SIM800_LQ);
                    res = 2;		// �յ�ERRORӦ��
                    goto finally;
                }
            }
            else
            {   // �޹̶�����Ӧ�𣬱�����������ʽ��⣬��������ʵ��
                LoopQue_Cpy(recbuf, &SIM800_LQ);
                res = 0;		// �յ���ȷ��Ӧ��
                goto finally;
            }
        }
        osDelay (10);
        waittime--;
    }
    if(waittime==0)
    {
        LoopQue_Cpy(recbuf, &SIM800_LQ);
        res=1; 				// ���ճ�ʱ
    }
finally:
    Catch_Pack_Flag = 0;
    LoopQue_init(&SIM800_LQ);
    return res;
}

int sim800_send_dat(char *cmd, char *ack, int waittime)
{
    s32 res = -1;



    return res;
}

s32 sim800_pack_analysis(struct LoopQue* lq)
{
    int i, idx_hp, hp;
    int strx= -1;
    int ts_cnt = 0;
    // ���һ������е����񣬲����Ⱥ�˳����Ϊ���ȼ�
    for(i=0; i< HANDLER_MAX; i++)
    {
        if(HandlerRegs[i].keyword == NULL)
            break;
        strx=LoopQue_Str(&SIM800_LQ, HandlerRegs[i].keyword);
        HandlerRegs[i].priority = strx;
        if(strx != -1)
            ts_cnt++;
    }

    if(ts_cnt == 0)
        return 0;

    // �������ȼ���������
    while(1)
    {
        hp = 32767;
        idx_hp = -1;
        for(i=0; i< HANDLER_MAX; i++)
        {
            if(HandlerRegs[i].keyword == NULL)
                break;
            if((HandlerRegs[i].priority != -1) && (HandlerRegs[i].priority < hp))
            {
                hp = HandlerRegs[i].priority;
                idx_hp = i;
            }
        }

        if(idx_hp != -1)
        {
            if(HandlerRegs[idx_hp].CallBack != NULL)
            {
                HandlerRegs[idx_hp].CallBack(&SIM800_LQ, &HandlerRegs[idx_hp]);
            }
            HandlerRegs[idx_hp].priority = -1;
        }
        else
        {   // ���������Ѵ������
            // ���û�����
            LoopQue_init(&SIM800_LQ);
            break;
        }
    }
    return ts_cnt;
}

const osThreadAttr_t sim800_attr =
{
    .stack_size = 1024 / 4,
    .priority = osPriorityBelowNormal,
};


__NO_RETURN void thread_sim800_rec (void *argument)
{
    int idle_cnt, status;
    u8 idx_dma_last,idx_dma;
    idle_cnt = 0;
    idx_dma_last = 0;
    Rx_Pack_Flag = 0;
    Catch_Pack_Flag = 0;

    LoopQue_init(&SIM800_LQ);
    DebugLog("THREAD:thread_sim800_rec RUNNING");
    while(1)
    {
        // ���ݰ�����״̬��
        switch(USART_RX_STATUS)
        {
        case 0:		// ״̬0������
            idx_dma = 256 - DMA_GetCurrDataCounter(DMA1_Channel6);
            if(idx_dma != idx_dma_last)
            {
                USART_RX_STATUS = 1;
            }
            else
            {
                osDelay (1);
            }
            break;
        case 1:		// ״̬1�����ݽ���
            idle_cnt = 0;
            while(idx_dma != idx_dma_last)
            {
                SIM800_LQ.buf[SIM800_LQ.buf_wp] = DMA1_CH6_BUF[idx_dma_last];
                SIM800_LQ.buf_wp++;
                idx_dma_last++;
            }
            SIM800_LQ.buf[SIM800_LQ.buf_wp] = 0;
            USART_RX_STATUS = 2;
            break;
        case 2:		// ״̬2����ʱ���
            idx_dma = 256 - DMA_GetCurrDataCounter(DMA1_Channel6);
            if(idx_dma == idx_dma_last)
            {
                idle_cnt++;
                if(idle_cnt > 20)
                {
                    USART_RX_STATUS = 0;
                    status = sim800_pack_analysis(&SIM800_LQ);
                    if(status == 0)
                    {
                        if(Catch_Pack_Flag == 1)
                        {   //�׳����ݰ�
                            Rx_Pack_Flag = 1;
                        }
                        else
                        {   // ���û�����������������ݰ�
                            LoopQue_init(&SIM800_LQ);
                        }
                    }
                }
                else
                {
                    osDelay (1);
                }
            }
            else
            {
                USART_RX_STATUS = 1;
            }
            break;
        default:
            USART_RX_STATUS = 0;
        }
    }
}
