#include "LED.H"
#include "stm32f10x.h"

/****************************************
**��ʼ��LED�������**
*****************************************/
void LedInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��LED0���ʱ��
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //LED0��������
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
	
		GPIO_SetBits(GPIOC,GPIO_Pin_6);//LED������1��Ĭ��LEDȫ��
}


