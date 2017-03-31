//#include "hw_spi_flash.h"//flash�洢
//#include "ff.h"          //fats�ļ�ϵͳ
#include <string.h>   
#include "usart1.h"        //����gps��Ϣ
#include "calendar.h"    //ʱ��
#include "delay.h"
#include "timer.h"         //100ms��ʱ��10hz����
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
	//usart1_init(9600);//����gps��Ϣ
	//RTC_Set(2017,03,31,23,42,40);
	
	while(1)
	{
	
	}
}
