//#include "hw_spi_flash.h"//flash存储
//#include "ff.h"          //fats文件系统
#include <string.h>   
#include "usart1.h"        //接收gps信息
#include "calendar.h"    //时钟
#include "delay.h"
#include "timer.h"         //100ms定时，10hz发送
#include "key.h"
#include "led.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LedInit();
	key_init();
	RTC_Init();
	
	// GPS daiceshi
	//usart1_init(9600);//接收gps信息
	//RTC_Set(2017,03,31,23,42,40);
	
	while(1)
	{
	
	}
}
