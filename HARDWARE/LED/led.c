#include "LED.H"
#include "stm32f10x.h"

/****************************************
**初始化LED输出引脚**
*****************************************/
void LedInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能LED0输出时钟
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //LED0引脚配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
	
		GPIO_SetBits(GPIOC,GPIO_Pin_6);//LED引脚置1，默认LED全灭
}


