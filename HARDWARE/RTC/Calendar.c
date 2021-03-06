#include "Calendar.H"
#include "delay.h"
#include "nongli.h"
#include "String.h"

tm Calendar;               //定义时间结构体 存储时间
u32 RandTime;

char ChinaYearString [5]= {0};
char ShenXiaoString  [3] = {0};
char JieQiString     [9] = {0};

u8 Is_Leap_Year(u16 year);//平年,闰年判断
u8 RTC_Get(void);         //更新时间


static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		        //RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级2位,从优先级2位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		       //使能该通道中断
    NVIC_Init(&NVIC_InitStructure);		       								 //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/**************************************
**实时时钟配置
**初始化RTC时钟,同时检测时钟是否工作正常
**BKP->DR1用于保存是否第一次配置的设置
**返回0:正常
**其他:错误代码
**************************************/
u8 RTC_Init(void)
{
    //检查是不是第一次配置时钟
    u8 temp=0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xfafa)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    {
        PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问
        BKP_DeInit();	//复位备份区域
        RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            delay_ms(10);
        }
        if(temp>=250)return 1;//初始化时钟失败,晶振有问题
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟
        RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        RTC_WaitForSynchro();		//等待RTC寄存器同步
        RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
        RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        RTC_EnterConfigMode();/// 允许配置
        RTC_SetPrescaler(32767); //设置RTC预分频的值
        RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        RTC_Set(2013,5,21,19,55,40);  //设置时间
        RTC_ExitConfigMode(); //退出配置模式
        BKP_WriteBackupRegister(BKP_DR1, 0xfafa);	//向指定的后备寄存器中写入用户程序数据
        PWR_BackupAccessCmd(DISABLE);  //禁止RTC后备寄存器的写
        RCC_ClearFlag();    //清楚RCC重启标志
    }
    else//系统继续计时
    {

        RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
        RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        PWR_BackupAccessCmd(DISABLE); //禁止RTC后备寄存器的写
        RCC_ClearFlag();              //清楚RCC重启标志
    }
    RTC_NVIC_Config();//RCT中断分组设置
    RTC_Get();//更新时间
    return 0; //ok

}
/***************************************
**RTC时钟中断
**每秒触发一次
**extern u16 tcnt;
***************************************/
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
    {
        RTC_Get();//更新时间
    }
    if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断
    }
    RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清闹钟中断
    RTC_WaitForLastTask();
}

/***************************************
**判断是否是闰年函数
**月份   1  2  3  4  5  6  7  8  9  10 11 12
**闰年   31 29 31 30 31 30 31 31 30 31 30 31
**非闰年 31 28 31 30 31 30 31 31 30 31 30 31
**输入:年份
**输出:该年份是不是闰年.1,是.0,不是
***************************************/
u8 Is_Leap_Year(u16 year)
{
    if(year%4==0) //必须能被4整除
    {
        if(year%100==0)
        {
            if(year%400==0)return 1;//如果以00结尾,还要能被400整除
            else return 0;
        } else return 1;
    } else return 0;
}

/***************************************
**设置时钟
**把输入的时钟转换为秒钟
**以1970年1月1日为基准
**1970~2099年为合法年份
**返回值:0,成功;其他:错误代码.
**月份数据表
***************************************/
u8 const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表
const u8 mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<1970||syear>2099)return 1;
    for(t=1970; t<syear; t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
        else seccount+=31536000;			  //平年的秒钟数
    }
    smon-=1;
    for(t=0; t<smon; t++)	 //把前面月份的秒钟数相加
    {
        seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
    }
    seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加
    seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
    seccount+=sec;//最后的秒钟加上去

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问
    RTC_SetCounter(seccount);	//设置RTC计数器的值

    RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
    return 0;
}

/***************************************
**得到当前的时间
**返回值:0,成功;其他:错误代码.
***************************************/
u8 RTC_Get(void)
{
    static u16 daycnt=0;
    char ChinaStr [15] = {0};
    u32 timecount=0;
    u32 temp=0;
    u16 temp1=0;
    u8  nongliDate[4];  //农历转换日期缓存
    timecount=RTC->CNTH;//得到计数器中的值(秒钟数)
    timecount<<=16;
    timecount+=RTC->CNTL;
    RandTime = timecount;//为了产生随机种子
    temp=timecount/86400;   //得到天数(秒钟数对应的)
    if(daycnt!=temp)//超过一天了
    {
        daycnt=temp;
        temp1=1970;	//从1970年开始
        while(temp>=365)
        {
            if(Is_Leap_Year(temp1))//是闰年
            {
                if(temp>=366)temp-=366;//闰年的秒钟数
                else {
                    temp1++;
                    break;
                }
            }
            else temp-=365;	  //平年
            temp1++;
        }
        Calendar.G_year=temp1;//得到年份
        temp1=0;
        while(temp>=28)//超过了一个月
        {
            if(Is_Leap_Year(Calendar.G_year)&&temp1==1)//当年是不是闰年/2月份
            {
                if(temp>=29)temp-=29;//闰年的秒钟数
                else break;
            }
            else
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
                else break;
            }
            temp1++;
        }
        Calendar.G_month=temp1+1;	//得到月份
        Calendar.G_date=temp+1;  	//得到日期
        GetWeek(Calendar.G_year,Calendar.G_month,Calendar.G_date,&Calendar.Week);//取星期几
        GetChinaCalendar(Calendar.G_year,Calendar.G_month,Calendar.G_date,nongliDate);//转换得到农历日期
        Calendar.L_year  = nongliDate[0]*100+nongliDate[1];
        Calendar.L_month = nongliDate[2];
        Calendar.L_date  = nongliDate[3];
        GetChinaCalendarStr(Calendar.G_year,Calendar.G_month,Calendar.G_date,ChinaStr);//获取农历年月日字符串
        GetShengXiaoStr(Calendar.G_year,Calendar.G_month,Calendar.G_date,ShenXiaoString );//获取生肖字符串
        GetJieQiStr(Calendar.G_year,Calendar.G_month,Calendar.G_date,JieQiString);  //获取节气字符串
        strncpy(ChinaYearString,ChinaStr,4);//将甲午拷贝到ChinaYearString
    }
    temp=timecount%86400;     		//得到秒钟数
    Calendar.Hour=temp/3600;     	//小时
    Calendar.Min=(temp%3600)/60; 	//分钟
    Calendar.Sec=(temp%3600)%60; 	//秒钟
    return 0;
}
