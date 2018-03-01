#include <stm32f10x.h>
#include <ausart.h>
#include "string.h"
#include <stdio.h>
#include "cmsis_os2.h"
#include "board.h"
#define GPS_IDLE			0
#define GPS_SYNC			1
#define GPS_HANDLE		2
#define GPS_TIMEOUT		3
#define GPS_OFFLINE		4
#define GPS_RX_LEN	256

typedef struct xTIME_STRUCT
{
    int tm_sec;   /* Seconds */
    int tm_min;   /* Minutes */
    int tm_hour;  /* Hour (0--23) */
    int tm_mday;  /* Day of month (1--31) */
    int tm_mon;   /* Month (0--11) */
    int tm_year;  /* Year (calendar year minus 1900) */
    int tm_wday;  /* Weekday (0--6; Sunday = 0) */
    int tm_yday;  /* Day of year (0--365) */
    int tm_isdst; /* 0 if daylight savings time is not in effect) */
} FF_TimeStruct_t;

extern uint8_t EmulateSerialBuf[GPS_RX_LEN]; //
extern uint16_t EmulateSerialLen ;
extern uint16_t EmulateSerialRx ;
extern uint16_t RemainDataNum;
uint8_t GpsBuf[GPS_RX_LEN]; //

static unsigned short res, idle, ticks, timeout;
static int  nc, longitude1, longitude2, latitude1, latitude2 ;
static float speed, ncf;
static char STA, NS, EW;
static FF_TimeStruct_t  tm;
uint8_t GPS_STATUS;

void thread_gps (void)
{
    unsigned short wp, rp, wp_last, i, rp_tmp, len;
    wp = rp = wp_last = i = rp_tmp = len = 0;
    char * end, *start;
    GPS_STATUS = GPS_OFFLINE;

    //打印线程开始

    while(1)
    {
        switch(GPS_STATUS)
        {
        case GPS_OFFLINE:
            ticks = 0;
            if(len > 0)
            {
                GPS_STATUS = GPS_IDLE;
                //上线
            }
            break;
        case GPS_IDLE:
            if(len < 8)
            {
                if(idle > 600)
                {
                    GPS_STATUS = GPS_OFFLINE;
                    //下线
                }
            }
            else
            {
                GPS_STATUS = GPS_SYNC;
                continue;
            }
            break;
        case GPS_SYNC:
            if(GpsBuf[0] != '$')
            {
                for(i=0; i < len; i++)
                {
                    if(GpsBuf[i] == '$')
                    {
                        len = 0;
                        break;
                    }
                }
                rp = (rp + i) % GPS_RX_LEN;
                GPS_STATUS = GPS_IDLE;
            }
            else
            {
                GPS_STATUS = GPS_HANDLE;
                continue;
            }
            break;
        case GPS_HANDLE:
            if(idle > 10)
            {
                GPS_STATUS = GPS_TIMEOUT;
            }
            end = strstr((char*)GpsBuf, "\r\n");
            if(end == NULL)
                break;
            end = end + 2;
            len = 0;//待修改
            //$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57
            if(strncmp((char*)GpsBuf, "$GPRMC", 6) == 0)
            {
                res = 0;
                start = (char*)GpsBuf;
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%2d%2d%2d.%2d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec, &nc);
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%c", &STA);//Status
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%d.%d", &latitude1, &latitude2); //
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%c", &NS);
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%d.%d", &longitude1, &longitude2); //
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%c", &EW);
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%f", &speed);
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%f", &ncf); //Course over ground
                start = strchr(start, ',') + 1;
                res += sscanf(start, "%2d%2d%2d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);

                if(res > 14 && STA == 'A') //
                {
                    /*
                    	数据存储
                    */
                    timeout = 60*100;
                    //LocationState = 1;
                    if(ticks == 0)
                    {
                        // sprintf((char*)GpsBuf, "%s %d°%d.%d′", (EW == 'E')?"东经":"西经", longitude1/100, longitude1%100, longitude2);
                        // DebugLog("GPS：%s", GpsBuf);
                        // sprintf((char*)GpsBuf, "%s %d°%d.%d′", (NS == 'N')?"北纬":"南纬", latitude1/100, latitude1%100, latitude2);
                        // DebugLog("GPS：%s", GpsBuf);
                    }
                    if(++ticks > 60)
                        ticks = 0;
                    tm.tm_year += 100;
                    tm.tm_mon  -= 1;
                    /*
                    	GPS 格式化时间
                    */
                }
            }
            rp = (rp + end - (char*)GpsBuf) % GPS_RX_LEN;
            break;
        case GPS_TIMEOUT:
            rp = wp;
            GPS_STATUS = GPS_IDLE;
            break;
        default:
            GPS_STATUS = GPS_IDLE;
            break;
        }

        if(timeout)
        {
            if(--timeout == 0)
            {
//                LocationState = 0;
                timeout = 60*100;
                if(GPS_STATUS == GPS_OFFLINE)
                {
                    // DebugLog("GPS：未连接");
                }
                else
                {
                    // DebugLog("GPS：无定位");
                }
            }
        }
//		if(TimeSync)
//        {
//            TimeSync--;
//        }
//			osDelay (10/ONE_TICK_MS);
				
        len = 0;
				
        wp = EmulateSerialRx;;
        rp_tmp = rp;
        while(wp != rp_tmp)
        {
            GpsBuf[len] = EmulateSerialBuf[rp_tmp];
            if(++rp_tmp >= GPS_RX_LEN)
                rp_tmp = 0;
            if(++len >= (GPS_RX_LEN-1))
                break;
        }
        GpsBuf[len] = 0;
        if(wp != wp_last)
        {
            wp_last = wp;
            idle = 0;
        }
        else
        {
            idle++;
        }
        osDelay (200);		//延时
    }
}






















