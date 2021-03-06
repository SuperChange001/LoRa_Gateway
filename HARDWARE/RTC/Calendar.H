#ifndef _CALENDAR_H__
#define _CALENDAR_H__

#include "stm32f10x.h"

/***时间结构体***/
typedef struct
{
    u8 Hour;
    u8 Min;
    u8 Sec;
    /*公历日月年*/
    u16 G_year;
    u8  G_month;
    u8  G_date;
    /*农历日月年*/
    u16 L_year;
    u8  L_month;
    u8  L_date;

    u8  Week;
} tm;
extern tm Calendar;
extern u32 RandTime;

extern char ChinaYearString[];
extern char JieQiString[];
extern char ShenXiaoString[];

u8 RTC_Init(void);        //初始化RTC,返回0,失败;1,成功;
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间



#endif



